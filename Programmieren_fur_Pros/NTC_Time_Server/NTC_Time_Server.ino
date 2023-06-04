/*
Project:  NTC Zeit Server for ESP32 and ESP8266
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 04.06.2023
Version:  V1.0
IDE:      Arduino IDE 2.1.0
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V3.1.2 
 - Board: esp32   by Espressif Systems   V2.0.9

Required libraries (sketch -> include library -> manage libraries)
 - 
*/

#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#include <time.h>

// Wi-Fi Settings
const char *wifi_ssid = "Deine_SSID";
const char *wifi_password = "Dein-WiFi-Passwort";
const char* NTP_SERVER = "ch.pool.ntp.org";
const char* TZ_INFO    = "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00";

char wochentage[7][12] = {"Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag"};
String monat[12] = {"Januar", "Februar", "März", "April", "Mai", "Juni", "Juli", "August", "September", "Oktober", "November", "Dezember"};

time_t now;
tm tm;

void setup () {
  Serial.begin(115200);
  setup_wifi();
  configTime(0, 0, NTP_SERVER);
  setenv("TZ", TZ_INFO, 1);
}

void loop () {
  time(&now);                       // Liest die aktuelle Zeit
  localtime_r(&now, &tm);           // Beschreibt tm mit der aktuelle Zeit
  
  Serial.printf("%02d-%02d-%04d \t", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
  //Serial.printf("%02d %s %04d \t", tm.tm_mday, monat[tm.tm_mon], tm.tm_year + 1900);  // Monat ausgeschrieben
  
  Serial.print(wochentage[tm.tm_wday]);
  //Serial.print(tm.tm_wday);         // Wochentag als Zahl, 0 = Sonntag
  Serial.print("\tder ");
  Serial.print(tm.tm_mday);
  Serial.print(" " + monat[tm.tm_mon] + " ");
  Serial.print(tm.tm_year + 1900);

  Serial.printf(" \tUhrzeit: %02d:%02d:%02d \n", tm.tm_hour, tm.tm_min, tm.tm_sec);
  delay(1000);
}

void setup_wifi() {
  delay(10);
  WiFi.begin(wifi_ssid, wifi_password);
  int counter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if (++counter > 100) ESP.restart();
    Serial.print(".");
  }
  Serial.println("WiFi connected");
}