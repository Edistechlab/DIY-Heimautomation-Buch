/*
Project:  ADS1115 Multiplexer Modul - Differenz messen
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 23.02.2023
Version:  V1.0
IDE:      Arduino IDE 2.0.3
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V3.1.1 

Required libraries (sketch -> include library -> manage libraries)
 - Adafruit ADS1X15 library V2.4.0
 - Adafruit BusIO library V1.14.1

Wirering for the ADS1115 Modul:
ADS1115     Wemos d1 mini    
VCC         3.3V / 5V    
GND         GND       
SCL         D1/5    
SDA         D2/4     
*/

#include <Adafruit_ADS1X15.h>
Adafruit_ADS1115 ads;
#define ADS1115_I2C_addr0 0x48

void setup(void) {
  Serial.begin(115200);
  ads.setGain(GAIN_TWOTHIRDS);
  if (!ads.begin(ADS1115_I2C_addr0)) {
    Serial.println("Finde keinen ADS1115");
    while (true);
  }
}

void loop(void) {
  int16_t result1, result2;
  float multiplier = 0.1875F;  // ADS1115  @ +/- 6.144V gain (16-bit results)
  result1 = ads.readADC_Differential_0_1();
  result2 = ads.readADC_Differential_2_3();
  Serial.print("Differenz 0 - 1: "); Serial.print(result1); Serial.print(" - ("); Serial.print(result1 * multiplier); Serial.println("mV)");
  Serial.print("Differenz 2 - 3: "); Serial.print(result2); Serial.print(" - ("); Serial.print(result2 * multiplier); Serial.println("mV)");
  delay(1000);
}
