#include <Arduino.h>
#include "M5StickCPlus.h"
#include "Wire.h"
#include "Adafruit_ADS1X15.h"

#define SDA_PIN 32
#define SCL_PIN 33
#define ADC_ADDR1 0x48 // GND  /**/

Adafruit_ADS1115 ads;
float Voltage = 0.0;

void setup() {
  M5.begin();
  // Serial.begin(115200);

  Wire.begin(SDA_PIN, SCL_PIN);
  ads.begin(ADC_ADDR1, &Wire);
}

void loop() {

  Serial.printf("ADC Value (A0-A4) : ");
  for(size_t i = 0; i < 4; ++i){
    int16_t adc_val = ads.readADC_SingleEnded(i);
    if(adc_val < 0) adc_val = 0;
    Serial.printf("%d\t", adc_val);
  }
  Serial.println();
  delay(100);
}
