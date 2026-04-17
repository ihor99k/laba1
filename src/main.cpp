
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
//#define ADC_res  12

//#define ADC_res_value  ((1 << ADC_res) - 1) // 4095 for 12-bit resolution
int adcValue = 0;
int16_t milivolts = 0;
int16_t ucalc = 0;
int8_t ADC_res =12;
int16_t ADC_res_value = 4095;

void setup() {
   Serial.begin(115200);
 // analogReadResolution(ADC_res);
  //analogSetPinAttenuation(ADC_PIN, ADC_0db);
}

void loop() {
  
   for (ADC_res = 9; ADC_res <= 12; ADC_res++) {
      delay(700);
      analogReadResolution(ADC_res);
  ADC_res_value = (1<<ADC_res) - 1; // 4095 for 12-bit resolution
   adcValue = analogRead(ADC_PIN);
   milivolts = analogReadMilliVolts(ADC_PIN);
  ucalc = adcValue * 3300 / ADC_res_value; // Calculate the voltage in millivolts based on the ADC value (assuming a 3.3V reference and 12-bit resolution)  

  Serial.print("ADC "+String(adcValue));
  Serial.print("  MILIVOLTS "+String(milivolts));
  Serial.print("  CALC "+String(ucalc));
  Serial.print("похибка: " + String(ADC_res) + "-bit: "  );
   Serial.print(String(static_cast<float>(ucalc) / static_cast<float>(milivolts) * 100.0f-100));
     Serial.println(" %");
   }
    delay(2000
   );
  }
