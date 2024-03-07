#include <ESP8266WiFi.h> // for ESP8266 boards

// Set your new MAC Address
uint8_t newMACAddress[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

void setup(){
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  wifi_set_macaddr(STATION_IF, &newMACAddress[0]); // for ESP8266
  delay(1000);
  Serial.print("MAC-Address: ");
  Serial.println(WiFi.macAddress());
}
 
void loop(){}