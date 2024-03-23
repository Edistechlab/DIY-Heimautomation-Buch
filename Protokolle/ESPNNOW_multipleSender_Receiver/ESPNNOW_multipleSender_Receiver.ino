/*
Project:  ESP-NOW multiple Sender - Empfänger 
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 17.03.2024
Version:  V1.0
IDE:      Arduino IDE 2.3.2
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: Arduino ESP32 Boards - by Arduino   V2.0.13

Required libraries (sketch -> include library -> manage libraries)
 - 
*/

#include <esp_now.h>
#include <WiFi.h>

// Define data structure
typedef struct struct_message {
  byte id;
  int humidity;
  int temperature;
} struct_message;

struct_message myData;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
}

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Received from: ");
  Serial.printf("Board %u: %u bytes\n", myData.id, len);
  Serial.printf("Humidity: %d \n", myData.humidity);
  Serial.printf("Temperature: %d \n", myData.temperature);
  Serial.println();
}

void loop() {
}
