/*
Project:  Reed Switch
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 26.11.2021
Version:  V1.0
IDE:      Arduino IDE 1.8.16
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V3.0.2 
 - Board: esp32   by Espressif Systems   V1.0.6

Required libraries (sketch -> include library -> manage libraries)       
*/

const int reedPin = 5;  // D1
bool switchState;
bool reedStatusOpen = false;

void setup() {
  Serial.begin(115200);
  pinMode(reedPin, INPUT);  
}

void loop() {
  switchState = digitalRead(reedPin);
  if (switchState == LOW) {
    if (reedStatusOpen == true) {
      Serial.println("Reed Switch is Closed.");
      reedStatusOpen = false;
    }
  } 
  else {
    if (reedStatusOpen == false) {
      Serial.println("Reed Switch is Open.");
      reedStatusOpen = true;
    }
  } 
  delay(100);
}
