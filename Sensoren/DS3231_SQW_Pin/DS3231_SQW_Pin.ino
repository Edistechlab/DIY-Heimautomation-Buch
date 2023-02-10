/*
Project:  DS3231 RTC - SQW Pin 1Hz Ausgabe
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 10.02.2023
Version:  V1.0
IDE:      Arduino IDE 2.0.3
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V3.1.1 

Required libraries (sketch -> include library -> manage libraries)
 - Adafruit RTClib library V2.1.1 / https://github.com/adafruit/RTClib
 - Adafruit BusIO library V1.13.2 / https://github.com/adafruit/Adafruit_BusIO

Wirering for the DS3231 RTC Modul:
DS3231      Wemos d1 mini    
VCC         3.3V          
GND         GND       
SCL         D1/5    
SDA         D2/4     
SQW         D4/2
*/

#include "RTClib.h"
RTC_DS3231 rtc;
const uint8_t alarmPin = 2;
volatile int sekunden = 0;
int TotalSekunden = 0;

void IRAM_ATTR onAlarmIntPin() {
  sekunden++;
}

void setup () {
  Serial.begin(115200);
  if (! rtc.begin()) {
    Serial.println("Finde keine RTC");
    while (true);
  }
  pinMode(alarmPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(alarmPin), onAlarmIntPin, FALLING);
  rtc.writeSqwPinMode(DS3231_SquareWave1Hz); // Jede Sekunde (1kHz, 4kHz, 8kHz)
}

void loop () {
  if (sekunden > 0) { 
    sekunden--;
    TotalSekunden++; 
    Serial.print("Gesamte Sekunden: ");
    Serial.println(TotalSekunden); 
  }
}


