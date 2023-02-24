/*
Project:  ADS1115 Multiplexer Modul - basiert auf dem Adafruit singleended Beispiel
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
  if (!ads.begin(ADS1115_I2C_addr0)) {
    Serial.println("Finde keinen ADS1115");
    while (true);
  }
}

void loop(void) {
  int16_t adc0, adc1, adc2, adc3;
  float volts0, volts1, volts2, volts3;

  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  adc2 = ads.readADC_SingleEnded(2);
  adc3 = ads.readADC_SingleEnded(3);

  volts0 = ads.computeVolts(adc0);
  volts1 = ads.computeVolts(adc1);
  volts2 = ads.computeVolts(adc2);
  volts3 = ads.computeVolts(adc3);

  Serial.println("-------------------------------");
  Serial.print("AIN0: "); Serial.print(adc0); Serial.print("  "); Serial.print(volts0); Serial.println(" V");
  Serial.print("AIN1: "); Serial.print(adc1); Serial.print("  "); Serial.print(volts1); Serial.println(" V");
  Serial.print("AIN2: "); Serial.print(adc2); Serial.print("  "); Serial.print(volts2); Serial.println(" V");
  Serial.print("AIN3: "); Serial.print(adc3); Serial.print("  "); Serial.print(volts3); Serial.println(" V");
  delay(1000);
}
