#include <Arduino.h>
#include <M5StickCPlus.h>
#include <Wire.h>
#include "ADS1115.h"

ADS1115 ads(0x48,&Wire);

void setup() {
  // put your setup code here, to run once:
  M5.begin();
  Wire.begin(32,33);
  ads.setGain(GAIN_TWOTHIRDS);
  ads.setRate(RATE_8SPS);

  // ads.startRead(MUX_SINGLE_0,true);
}

void loop() {
  // put your main code here, to run repeatedly:
  static uint32_t last_time = millis();
  if(millis() - last_time > 500)
  {
    last_time = millis();

    Serial.printf("Current gain : %f\tCurrent rate : %d\n",ads.getGain(),ads.getRate());
    for(uint8_t ch=0; ch < 4; ch++)
      Serial.printf("channel%d : %f\t",ch,ads.readChannel(ch));
    Serial.println();
    Serial.printf("Diff0_1 : %f\t",ads.readDiff_0_1());
    Serial.printf("Diff0_3 : %f\t",ads.readDiff_0_3());
    Serial.printf("Diff1_3 : %f\t",ads.readDiff_1_3());
    Serial.printf("Diff2_3 : %f\n",ads.readDiff_2_3());
    Serial.println("-----");
    Serial.println();

    // Serial.println(ads.toVolt(ads.getLastConversion()));
  }
}