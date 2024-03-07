#include <WiFi.h> // for ESP32 boards
//#include <ESP8266WiFi.h> // for ESP8266 boards

void setup(){
  Serial.begin(115200);
  delay(1000);
  Serial.print("MAC-Address: ");
  Serial.println(WiFi.macAddress());
}
 
void loop(){}