/*
Project:  ESP-NOW Sender
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

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = {0x34, 0x85, 0x18, 0x7B, 0xF1, 0x14};
esp_now_peer_info_t peerInfo;

const byte buttonPin = 2; //D2
byte lastButtonState = LOW;
byte buttonState = LOW;
const byte debounceTime = 30;
unsigned long lastButtonTimeChange = 0;

// Define data structure
typedef struct struct_message {
  byte a;
} struct_message;

struct_message myData;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP);
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false; 
 
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  if (millis() - lastButtonTimeChange > debounceTime) {
    buttonState = digitalRead(buttonPin);
    if (buttonState != lastButtonState) {
      lastButtonTimeChange = millis();
      lastButtonState = buttonState;
      if (buttonState == LOW) {
        Serial.printf("Taster wurde gedrückt!\n");
        myData.a = !buttonState;
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
        delay(10);
      }
      if (buttonState == HIGH) {
        Serial.printf("Taster wurde losgelassen!\n");
        myData.a = !buttonState;
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
        delay(10);
      }
    }
  }
}
