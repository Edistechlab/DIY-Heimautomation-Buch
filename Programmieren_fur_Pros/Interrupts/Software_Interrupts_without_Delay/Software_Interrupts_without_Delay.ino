/*
Project:  Software Interrupt  - LED schaltet alle Sekunden
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 27.05.2023
Version:  V1.0
IDE:      Arduino IDE 2.1.0
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V3.1.2 
 - Board: esp32 by Espressif   V2.0.9 

Required libraries (sketch -> include library -> manage libraries)
*/

const byte LEDPin = 4; //D2  - Wemos d1 mini
byte toggleState = 0;
long previousMillis = 0;
long interval = 1000;  // 1000 Millisekunden = 1 Sekunde

void setup() {
  Serial.begin(115200);
  pinMode(LEDPin, OUTPUT);
}

void loop() {
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;   
    toggleState =! toggleState;
    digitalWrite(LEDPin, toggleState);
  }
}