/*
Project:  ESP-NOW Sender for ESP8266/Receiver 01 
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 17.03.2024
Version:  V1.0
IDE:      Arduino IDE 2.3.2
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: Arduino ESP32 Boards - by Arduino   V2.0.13

Required libraries (sketch -> include library -> manage libraries)
 - 
*/

#include <espnow.h>
#include <ESP8266WiFi.h>

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = {0x24, 0x62, 0xAB, 0x1C, 0x06, 0xA4};

// Define data structure
typedef struct struct_message_sent {
  int humidity;
  int temperature;
} struct_message_sent;

typedef struct struct_message_receive {
  char text[32];
} struct_message_receive;

struct_message_sent myDataSent;
struct_message_receive myDataReceive;

// callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t status) {
  Serial.print("\r\nLast Packet Sent Status:\t");
  Serial.println(status == 0 ? "Delivery Success" : "Delivery Fail");
}

// callback function that will be executed when data is received
void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&myDataReceive, incomingData, sizeof(myDataReceive));
  Serial.print("Message: ");
  Serial.println(myDataReceive.text);
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO); // for ESP8266 - set role
  esp_now_register_recv_cb(OnDataRecv);
  esp_now_register_send_cb(OnDataSent);
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 0, NULL, 0);
}

void loop() {
  myDataSent.humidity = random(0,99);
  myDataSent.temperature = random(0,50);

  // Send message via ESP-NOW
  esp_now_send(broadcastAddress, (uint8_t *) &myDataSent, sizeof(myDataSent));
  delay(10000);
}
