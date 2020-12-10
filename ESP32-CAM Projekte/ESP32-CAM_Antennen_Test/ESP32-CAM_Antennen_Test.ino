/*
Project:  Wi-Fi Signalstärke messen mit dem ESP32-CAM Modul
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 10.12.2020
Version:  V1.0
IDE:      Arduino IDE 1.8.13
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp32   by Espressif Systems   V1.0.4
*/

#include "WiFi.h"
#define wifi_ssid "Your_SSID"
#define wifi_password "Your_Password"

void setup(){
  Serial.begin(115200);
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("");
}
 
void loop(){
  Serial.print("- Signal Level: ");
  Serial.println(WiFi.RSSI());
  delay(2000);
}
