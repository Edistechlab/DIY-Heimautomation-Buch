/*
Project:  Micro-SD Card Module - ESP32 Webserver
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 02.06.2024
Version:  V1.0
IDE:      Arduino IDE 2.3.
Board:    Arduino Nano ESP32
Pins:     CS=D10 / MOSI=D11 / MISO=D12 / SCL=D13 - Arduino Pins
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp32 by Espressif Systems   V3.0.0 

Required libraries (sketch -> include library -> manage libraries)
 - ESPAsyncWebServer by lacamera V3.1.0
 - AsyncTCP by dvarrel V1.1.4
 - ESPAsyncTCP by dvarrel V1.2.4
*/

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <SD.h>

const uint8_t chipSelect = 10;

// Wi-Fi Settings
const char *wifi_ssid = "Your_SSID";
const char *wifi_password = "Your_Password";

AsyncWebServer webServer(80);  // Create AsyncWebServer object on port 80

void setup() {
  Serial.begin(115200);
  setup_wifi();
  
  // init MicroSD Card
  if (!SD.begin(chipSelect)) {
    Serial.println("Karte konnte nicht gefunden werden!");
    return;
  }

  webServer.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SD, "/index.html", "text/html");
  });
  webServer.serveStatic("/", SD, "/");
  webServer.begin();
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
} 

void loop() {
  // Your code here
}
