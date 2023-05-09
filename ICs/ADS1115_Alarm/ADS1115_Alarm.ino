/*
Project:  ADS1115 Multiplexer Modul - Interrupt-Alarm
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 26.02.2023
Version:  V1.0
IDE:      Arduino IDE 2.0.4
 
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
ALRT        D5     
*/

#include <Adafruit_ADS1X15.h>
Adafruit_ADS1115 ads;
const uint8_t alarmPin = 14;  //D5
const uint8_t LEDPin = 12;    //D6
#define ADS1115_I2C_addr0 0x48

void IRAM_ATTR onAlarmIntPin() {
  // Diese Funktion wird mit dem Alarm Pin aufgerufen
  Serial.println("Alarm wurde am Interrupt Pin ausgelöst!");
  digitalWrite(LEDPin, HIGH);
}

void setup(void) {
  Serial.begin(115200);
  if (!ads.begin(ADS1115_I2C_addr0)) {
    Serial.println("Finde keinen ADS1115");
    while (true);
  }
  pinMode(LEDPin, OUTPUT); 
  pinMode(alarmPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(alarmPin), onAlarmIntPin, FALLING);
  ads.startComparator_SingleEnded(1, 10666); // 2V : 0,1875mV 
}

void loop(void) {
  int16_t adc1;
  // Komparator wird nur nach einem Lesevorgang zurückgesetzt
  adc1 = ads.getLastConversionResults();
  Serial.print("AIN1: "); Serial.println(adc1);
  if (adc1<=10666) {digitalWrite(LEDPin, LOW);}
  delay(100);
}
