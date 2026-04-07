
#include <Arduino.h>

#define GREEN 8
#define BLUE 9
#define YELLOW 10
#define RED 11
#define WHITE 13
//#define BUZZER 5

uint32_t delayTime = 500;
double devider = 0.99;
int16_t ledPins[] = {GREEN, BLUE, YELLOW, RED, WHITE};
int16_t randomLed;
void setup() {
  for (int i = 0; i < 5; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
  //pinMode(BUZZER, OUTPUT);
  for (int i = 0; i < 5; i++) {
    digitalWrite(ledPins[i], HIGH);
  }
}

void loop() {
  while (delayTime>4)
  {
    randomLed = random(5);
    digitalWrite(ledPins[randomLed], LOW);
    delay(delayTime);
    digitalWrite(ledPins[randomLed], HIGH);
    delayTime=delayTime*devider;
  }
  
  if (delayTime < 5) {
        delayTime = 500;
      }

}
