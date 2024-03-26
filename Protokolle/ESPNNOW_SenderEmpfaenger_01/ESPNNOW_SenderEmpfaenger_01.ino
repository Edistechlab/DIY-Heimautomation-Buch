/*
Project:  ESP-NOW Sender/Receiver 01 
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

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = {0x34, 0x85, 0x18, 0x7B, 0xCE, 0x7C};
esp_now_peer_info_t peerInfo;

// Define data structure
typedef struct struct_message_send {
  int humidity;
  int temperature;
} struct_message_send;

typedef struct struct_message_receive {
  char text[32];
} struct_message_receive;

struct_message_send myDataSend;
struct_message_receive myDataReceive;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&myDataReceive, incomingData, sizeof(myDataReceive));
  Serial.print("Message: ");
  Serial.println(myDataReceive.text);
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
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
  myDataSend.humidity = random(0,99);
  myDataSend.temperature = random(0,50);

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myDataSend, sizeof(myDataSend));
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(10000);
}
