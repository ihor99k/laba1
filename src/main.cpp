
#include <Arduino.h>




enum class PinState : uint8_t {
  Off = LOW,
  On = HIGH
};
enum class BlinkTypes : uint8_t {
  Off,
  On,
  Blink
};

struct ledconfig {
  uint8_t pin;
  uint16_t BlinkDelay;
  uint8_t BlinkTimes;
  
};
struct buttonConfig {  //налаштуваня кнопки в одному місці
    uint8_t pin;
    uint8_t pull;
    uint8_t interruptType;
    uint16_t debounceDelay ;
  };
constexpr buttonConfig Button1config{6, LOW , RISING, 100};  //ініціалізація Кнопки з констекспер
constexpr ledconfig Led1Config{ //ініціалізація Леду з констекспер
                    11,    // pin
                    500,  // blinkDelay
                    5     // blinkTimes
  }; 

class LED {
  BlinkTypes blinkType = BlinkTypes::Off;
  uint8_t pin;
  volatile PinState state = PinState::Off; 
  public:
    PinState getState() const { return state; }
    void init(uint8_t p) { pin = p; pinMode(pin, OUTPUT); }

    void set(PinState state) {
      digitalWrite(pin, state == PinState::On ? HIGH : LOW);
      this->state = state;
    }
     void toggle() {
      if (blinkType == BlinkTypes::Off) {
        blinkType = BlinkTypes::On;
      }
      else if (blinkType == BlinkTypes::On) {
        blinkType = BlinkTypes::Blink;
      }
       else if (blinkType == BlinkTypes::Blink) {
        blinkType = BlinkTypes::Off;
       }
       else {
        blinkType = BlinkTypes::Off;
       }
     }
     BlinkTypes getBlinkType() const { return blinkType; }
};



 

  class Button1 {
    buttonConfig config;
    volatile bool interruptState = false;
    uint32_t lastPressTime = 0;
    public:
      
      void configAsInput(const buttonConfig& cfg) {
        config = cfg;
        pinMode(config.pin, config.pull == HIGH ? INPUT_PULLUP : INPUT_PULLDOWN);
        }
      
      void configAsInterrupt (const buttonConfig& cfg) {
          config = cfg;
          pinMode(config.pin, config.pull == HIGH ? INPUT_PULLUP : INPUT_PULLDOWN);
          attachInterruptArg(digitalPinToInterrupt(config.pin), isrHandler, this, config.interruptType) ;
        }
      bool wasPressed() {
        bool flag = false;

        noInterrupts();
        flag = interruptState;
        interruptState = false;
        interrupts();

        if (!flag) {
          return false;
        }

        uint32_t now = millis();

        if ((now - lastPressTime) < config.debounceDelay) {
          return false;
        }

        lastPressTime = now;
        return true;
      
      }
    
    private:
      static void IRAM_ATTR isrHandler(void* arg) {
        Button1* btn = static_cast<Button1*>(arg);
        btn->buttonOnInterrupt();
      }

      void IRAM_ATTR buttonOnInterrupt() {
        interruptState = true;
      }


  };

  LED led_1;
  Button1 button_1;

  void setup() {
    led_1.init(Led1Config.pin);
    button_1.configAsInterrupt(Button1config);
    Serial.begin(115200);
  }
 void loop() {
    static uint32_t timestamp = 0;
    static uint32_t lastMeasureTime = micros();
    static uint32_t loops = 0;
    loops++;

     uint32_t now = millis();

     if (button_1.wasPressed()) {
        led_1.toggle();
      }

     switch (led_1.getBlinkType()) {
        case BlinkTypes::Off:
          led_1.set(PinState::Off);
          break;;
        case BlinkTypes::On:
          led_1.set(PinState::On);
          break;
        case BlinkTypes::Blink:
            if (now - timestamp >= Led1Config.BlinkDelay) {
            timestamp = now;
            if (led_1.getState() == PinState::Off) {
              led_1.set(PinState::On);
              } 
            else {  
              led_1.set(PinState::Off);
                }
            }
             break;
           }
       
   
  if (loops >= 1000) {
    uint32_t now = micros();
    uint32_t elapsed = now - lastMeasureTime;

    float loopTimeUs = (float)elapsed / loops;
  

    Serial.print("Loop time: ");
    Serial.print(loopTimeUs);
    Serial.print(" us ");
    loops = 0;
    lastMeasureTime = now;
  }
  }