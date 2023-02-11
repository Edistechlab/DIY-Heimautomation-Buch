/*
Project:  DS3231 RTC - Grundlagen
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 31.10.2022
Version:  V1.0
IDE:      Arduino IDE 2.0.1
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V3.0.2 
 - Board: esp32   by Espressif Systems   V1.0.6

Required libraries (sketch -> include library -> manage libraries)
 - Adafruit RTClib library V2.1.1 / https://github.com/adafruit/RTClib
 - Adafruit BusIO library V1.13.2 / https://github.com/adafruit/Adafruit_BusIO

Wirering for the DS3231 RTC Modul:
DS3231      ESP8266    ESP32       UNO       Mega
VCC         3.3V       3.3V        3.3V      3.3V      
GND         GND        GND         GND       GND
SCL         D1/5       22          A5        21
SDA         D2/4       21          A4        20
*/

#include "RTClib.h"
RTC_DS3231 rtc;

char wochentage[7][12] = {"Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag"};

void setup () {
  Serial.begin(115200);
  if (! rtc.begin()) {
    Serial.println("Finde keine RTC");
    while (true);
  }

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // Zeit vom Compiler
  //rtc.adjust(DateTime(2022, 11, 21, 13, 0, 0)); // J, M, T, Std, Min, Sek
}

void loop () {
  DateTime jetzt = rtc.now();
  Serial.print(jetzt.day(), DEC);
  Serial.print('.');
  Serial.print(jetzt.month(), DEC);
  Serial.print('.');
  Serial.print(jetzt.year(), DEC);
  Serial.print(" ");
  Serial.print(wochentage[jetzt.dayOfTheWeek()]);
  Serial.print(" ");
  Serial.print(jetzt.hour(), DEC);
  Serial.print(':');
  Serial.print(jetzt.minute(), DEC);
  Serial.print(':');
  Serial.print(jetzt.second(), DEC);
  Serial.println();
  //Formatierung vom Datum und der Zeit
  char buf1[] = "DD.MMM.YYYY-hh:mm:ss";
  Serial.println(jetzt.toString(buf1));
  delay(3000);
}
