/*
Project:  ESP-NOW Empfänger
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 06.03.2024
Version:  V1.0
IDE:      Arduino IDE 2.3.2
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: Arduino ESP32 Boards - by Arduino   V2.0.13

Required libraries (sketch -> include library -> manage libraries)
 - 
*/

#include <esp_now.h>
#include <WiFi.h>

const byte LEDPin = 2; //D2

// Define data structure
typedef struct struct_message {
  byte a;
} struct_message;

struct_message myData;

void setup() {
  Serial.begin(115200);
  pinMode (LEDPin, OUTPUT);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
}

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Button: ");
  Serial.println(myData.a);
}

void loop() {
  if (myData.a == 0) {digitalWrite(LEDPin, LOW);}
  if (myData.a == 1) {digitalWrite(LEDPin, HIGH);}
}
