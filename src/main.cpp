
#include <Arduino.h>
/*
#define GREEN 8
#define BLUE 9
#define YELLOW 10
#define RED 11
#define WHITE 13
#define BUZZER 5
#define BUTTON 36
#define ZERO 0*/
#define ADC_PIN 1


int adcValue = 0;
int16_t milivolts = 0;
int16_t ucalc = 0;


void setup() {
   Serial.begin(115200);
 
}

void loop() {
  adcValue = analogRead(ADC_PIN);
   milivolts = analogReadMilliVolts(ADC_PIN);
  ucalc = adcValue * 3300 / 4095; // Calculate the voltage in millivolts based on the ADC value (assuming a 3.3V reference and 12-bit resolution)  

  Serial.println(adcValue);
  Serial.println(milivolts);
  Serial.println(ucalc);
  Serial.print("похибка: ");
   Serial.print(String(static_cast<float>(ucalc) / static_cast<float>(milivolts) * 100.0f-100));
     Serial.println(" %");
    delay(1000
   );
  }
