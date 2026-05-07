
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
//constexpr buttonConfig Button1config{99, LOW , RISING, 100};  //ініціалізація Кнопки з констекспер
constexpr ledconfig Led1Config[3]{ {12, 200, 5}, {13, 500, 5}, {14, 1000, 5} }; // ініціалізація Леду з констекспер
constexpr uint8_t led_count = std::size(Led1Config);


class LED {
  BlinkTypes blinkType = BlinkTypes::Off;
  uint8_t pin;
  ledconfig config;
  volatile PinState state = PinState::Off; 
  uint32_t tstamt_inter = 0;
  public:
    PinState getState() const { return state; }
    void init(ledconfig cfg) {
      config = cfg;
      pinMode(config.pin, OUTPUT); }

    void set(PinState state) {
      digitalWrite(config.pin, state == PinState::On ? HIGH : LOW);
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

     void blink(uint32_t now1) {
        if (now1 - tstamt_inter >= config.BlinkDelay) {
            tstamt_inter = now1;
           set(state == PinState::On ? PinState::Off : PinState::On);
           //Serial.println("LED blinked");
           
        }

     }
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
          attachInterruptArg(digitalPinToInterrupt(config.pin), isrHandler, this, config.interruptType) ; //ця функція дозволяє прив'язати обробник переривання до конкретного піну та передати в нього аргумент (у цьому випадку, вказівник на об'єкт Button1). Це дозволяє обробнику отримувати доступ до стану кнопки та виконувати необхідні дії при спрацьовуванні переривання.
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

  LED led_1[led_count];
  //Button1 button_1;

  void setup() {
    for (uint8_t i = 0; i < led_count; i++) {
     led_1[i].init(Led1Config[i]);
   }
    //Serial.begin(115200);
   // pinMode(13, OUTPUT);
  }
 void loop() {
   uint32_t now = millis();
for (uint8_t i = 0; i < led_count; i++) {
    led_1[i].blink(now);
    
   }
  
  }