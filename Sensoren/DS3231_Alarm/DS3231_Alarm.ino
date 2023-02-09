/*
Project:  DS3231 RTC - Alarm
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 08.02.2023
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

void IRAM_ATTR onAlarmIntPin() {
  // Diese Funktion wird mit dem SQW Pin bei einem Alarm aufgerufen
  Serial.println("Alarm wurde am Interrupt Pin ausgelöst!");
}
void setup () {
  Serial.begin(115200);
  if (! rtc.begin()) {
    Serial.println("Finde keine RTC");
    while (true);
  }
  
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // Zeit vom Compiler
  //rtc.adjust(DateTime(2022, 2, 7, 20, 56, 0)); // J, M, T, Std, Min, Sek

  pinMode(alarmPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(alarmPin), onAlarmIntPin, FALLING);
  
  //Alarm register löschen und beide Alarme vorerst ausschalten  
  rtc.disableAlarm(1);
  rtc.disableAlarm(2);
  rtc.clearAlarm(1);
  rtc.clearAlarm(2);  
  rtc.writeSqwPinMode(DS3231_OFF);

  // Alarm 1 und 2 setzen
  rtc.setAlarm1(rtc.now() + TimeSpan(0, 0, 0, 10), DS3231_A1_Second); // löst jede Minute von jetzt plus 10 Sek. aus.
  rtc.setAlarm2(DateTime(0, 0, 0, 21, 41, 0), DS3231_A2_Hour);  // löst um 21:41:00 aus
}

void loop () {
  if(rtc.alarmFired(1)) {
    rtc.clearAlarm(1);
    Serial.println("Alarm 1 ausgelöst über alarmFired Register");
    Serial.println();
  }

  if(rtc.alarmFired(2)) {
    rtc.clearAlarm(2);
    Serial.println("Alarm 2 ausgelöst über alarmFired Register");
    Serial.println();
  }
}


