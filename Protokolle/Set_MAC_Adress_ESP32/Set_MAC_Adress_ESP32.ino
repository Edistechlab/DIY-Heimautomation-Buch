#include <WiFi.h> // for ESP32 boards
#include <esp_wifi.h> // for ESP32 boards

// Set your new MAC Address
uint8_t newMACAddress[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

void setup(){
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  esp_wifi_set_mac(WIFI_IF_STA, &newMACAddress[0]);  //for ESP32
  delay(1000);
  Serial.print("MAC-Address: ");
  Serial.println(WiFi.macAddress());
}
 
void loop(){}