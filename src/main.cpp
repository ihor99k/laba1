
#include <Arduino.h>

#define GREEN 8
#define BLUE 9
#define YELLOW 10
#define RED 11
#define WHITE 13
#define BUZZER 5
#define BUTTON 36
#define ZERO 0

uint32_t delayTime = 500;
double devider = 0.99;
int16_t ledPins[] = {GREEN, BLUE, YELLOW, RED, WHITE};
int16_t randomLed;

int16_t buttons_PU[] = {ZERO, BUTTON};
int16_t buttons_State[] = {0, 0};
uint32_t targetmils[] = {0, 0};
int8_t initstate_fkmk = 0;
int buttonState = 0;
void setup() {
  for (int i = 0; i < 5; i++) {
    //digitalWrite(ledPins[i], HIGH);
    pinMode(ledPins[i], OUTPUT);
  }
  //pinMode(BUZZER, OUTPUT);
  
    pinMode(BUTTON, INPUT_PULLUP);
    targetmils[1] = millis();
  targetmils[0] = millis();
 
  for (int i = 0; i < 5; i++) {
    digitalWrite(ledPins[i], HIGH);
  }
}

void loop() {
  if (initstate_fkmk == 0)
  {
    pinMode(buttons_PU[0], INPUT_PULLUP);
    initstate_fkmk = 1;
  }
  
  /*while (delayTime>4)
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
     */
    for (int i = 0; i < 2; i++) {
  int currentState = digitalRead(buttons_PU[i]);

  if (currentState == HIGH) { 
    // Кнопка відпущена — скидаємо таймер і записуємо HIGH
    targetmils[i] = millis();
    buttons_State[i] = HIGH;
  } 
  else {
    // Кнопка натиснута (LOW) — перевіряємо, чи пройшло 20 мс
    if (millis() - targetmils[i] > 20) {
      buttons_State[i] = LOW; // Дребезг пройшов, фіксуємо натискання
    }
  }
}


      if (buttons_State[1] == LOW) {
         digitalWrite(BLUE, LOW); //
         delay(100);
         digitalWrite(BLUE, HIGH); //
         delay(100);

      } else {
        
        digitalWrite(BLUE, HIGH);
      }
if (buttons_State[0] == LOW) {
         digitalWrite(WHITE, LOW); //
         delay(1000);
         digitalWrite(WHITE, HIGH); //
         delay(1000);

      } else {
        
        digitalWrite(WHITE, HIGH);
      }


      digitalWrite(YELLOW, LOW);
   
  }
