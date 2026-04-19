
#include <Arduino.h>
#define GREEN 9
#define GreenTime 10000
#define YELLOW 10
#define YellowTime 2000
#define BLINKYELLOW 500
#define RED 11
#define RedTime 10000
#define RedYellowTime 700
#define crossGreen 4 //сигнал, що перехресна дорога в дозволеному стані
#define weGreen 5 //сигнал, що наша дорога в дозволеному стані
#define Resolution 8 //розрядність АЦП для  регулювання яскравості
#define PWM_FR 5000 //частота ШІМ для регулювання яскравості
#define ENABLE 21 //кнопка для всього
#define ADC_PIN 1 //датчик освітленості 
#define DEBOUNCE 50 //затримка для дебаунсу кнопки
#define GreenBlinkTime 2000


//uint16_t Light = 0;
uint32_t timestamp = 0;
enum TrafficLightState {
  OFF_STATE,
  RED_STATE,
  YELLOW_STATE,
  GREEN_STATE,
  GREEN_BLINK_STATE, 
  RED_YELLOW_STATE,
  BLiNK_YELLOW_STATE,
  LAST_STATE
  
};
volatile TrafficLightState currentState = OFF_STATE; // в перериванні юзаю
 
uint8_t lastButton =LOW;

uint8_t Button (uint8_t pin,uint8_t pull=HIGH, uint16_t debounceDelay = 20) {  //дебаунс з врахуванням пул ап/давн і затримки для стабілізації сигналу кнопки, дебаунс відпущеної ігноруємо
    static uint32_t targetmils = 0;
    uint8_t currentState = digitalRead(pin);
    static uint8_t buttons_State = LOW;
    
    if (currentState == pull) { 
      // Кнопка відпущена — скидаємо таймер і записуємо HIGH
      targetmils = millis();
      buttons_State = LOW;
    } 
    else {
      // Кнопка натиснута (LOW) — перевіряємо, чи пройшло 20 мс
      if (millis() - targetmils > debounceDelay) {
        buttons_State = HIGH; // Дребезг пройшов, фіксуємо натискання
      }
    }
    return buttons_State;
}
uint8_t buttonState = Button(ENABLE, HIGH, DEBOUNCE);

int8_t SwitchState (uint32_t duration, bool reset = false) { //таймер для визначення моменту зміни стану світлофора, якщо reset==true, то таймер буде скинутий і почне відлік заново

  static uint32_t targetAction = 0;
  static bool newcircle = true;
    if (reset) {
      newcircle = true;
      return 0;
    }
  if (newcircle){
      targetAction = millis() + duration;
      newcircle = false;
      return 0;
  }
  else if (millis() >= targetAction) {
      newcircle = true;
      return 1;
  }
  else {
      return 0;
  }
}

class Light {
  uint16_t pinNumber;
  uint16_t brightness = 0;
public:
  Light(uint16_t p) : pinNumber(p) {}
  void begin() { ledcAttach(pinNumber, PWM_FR, Resolution); }
  void set(uint16_t value) { brightness = value; ledcWrite(pinNumber, value); }
  void off() { set(0); }
  void blink(uint16_t value, uint16_t period = 500) {
    static uint32_t t = 0;
    static bool on = false;
    if (millis() - t >= period) { t = millis(); on = !on; }
    set(on ? value : 0);
  }
};

Light red(RED), yellow(YELLOW), green(GREEN);

uint8_t LightADC(uint8_t pin) {   //перевірка ADC з інтервалом
  static uint32_t lastRead = 0;
  static uint8_t cached = 80;     

  if (millis() - lastRead >= 1000) {
    lastRead = millis();
    cached = analogRead(pin);
    if (cached < 80) cached = 80;
  }
  return cached;
}

volatile bool crossIsGreen = false;
volatile bool needReset = false;

void IRAM_ATTR onCrossChange() {      
  crossIsGreen = digitalRead(crossGreen);
  if (crossIsGreen) {  //подаєм 3.3 на пін і спрацьовує
    if (currentState == GREEN_STATE || currentState == GREEN_BLINK_STATE //скидання при конфлікті
        ||currentState == OFF_STATE||currentState == BLiNK_YELLOW_STATE) { // скидання з стартових станів
      currentState = YELLOW_STATE;
      needReset = true;
    }
  }
}

void setup() {
   Serial.begin(115200);
    red.begin();
    yellow.begin();
    green.begin();
    pinMode(crossGreen, INPUT_PULLDOWN);
    attachInterrupt(crossGreen, onCrossChange, RISING);
    digitalWrite(weGreen, LOW);
    pinMode(weGreen, OUTPUT);
    pinMode(ENABLE, INPUT_PULLUP);
    analogReadResolution(Resolution);
    
}

void loop() {
   buttonState = Button(ENABLE, HIGH, DEBOUNCE);
  //тест дебаунсу кнопки
  if (buttonState == HIGH && lastButton == LOW) {
      currentState = static_cast<TrafficLightState>(currentState + 1);
      Serial.print("Button pressed, switching to state: ");
      Serial.println(currentState);
      if (currentState >= TrafficLightState::LAST_STATE) {
        currentState = OFF_STATE; 
      }
      lastButton = HIGH;
      } 
    else if (buttonState == LOW) {
      lastButton = LOW;
      }

    uint8_t lightLevel = LightADC(ADC_PIN);
    
    if (needReset) { SwitchState(0, true); needReset = false; }

   switch (currentState) {
    case OFF_STATE:
      red.off();
      yellow.off();
      green.off();
      break;  
    case RED_STATE:
      red.set(lightLevel);
      yellow.set(0);
      green.set(0);
      if (SwitchState(RedTime) == 1) currentState = RED_YELLOW_STATE;
      break;                
    case YELLOW_STATE:
      red.set(0);
      yellow.set(lightLevel);
      green.set(0);
      if (SwitchState(YellowTime) == 1) currentState = RED_STATE;
        break;
    case GREEN_STATE:
        red.set(0);
        yellow.set(0);
        green.set(lightLevel);
        if (SwitchState(GreenTime)==1) currentState = GREEN_BLINK_STATE;
      break;
    case GREEN_BLINK_STATE:
      red.off();
      yellow.off();
      green.blink(lightLevel, 500);  
      if (SwitchState(GreenBlinkTime)==1) currentState = YELLOW_STATE;
      break;
    case RED_YELLOW_STATE:
      red.set(lightLevel);
      yellow.set(lightLevel);
      green.set(0);
      
      if (SwitchState(RedYellowTime) == 1 && !digitalRead(crossGreen) 
                      ) currentState = GREEN_STATE;
      break;
    case BLiNK_YELLOW_STATE: {
      red.off();
      green.off();
      yellow.blink(lightLevel, 500);
      break;
      }
  }
  bool weActive = (currentState == GREEN_STATE || currentState == GREEN_BLINK_STATE);
  digitalWrite(weGreen, weActive ? HIGH : LOW); 
}
