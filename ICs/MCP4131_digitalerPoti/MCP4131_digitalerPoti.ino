/*
Project:  Digitaler Potenziometer - MCP4131 
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 25.10.2023
Version:  V1.0
IDE:      Arduino IDE 2.1.1
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: Arduino ESP8266 Boards - by ESP8266 Community   V3.1.2

Required libraries (sketch -> include library -> manage libraries)
 - 
*/

#include <SPI.h>

const byte CSPin = 8;
const byte maxPotiSteps = 128;  // 0 - 128 or 0 - 255 Steps
byte address = 0x00;

void setup() {
  pinMode (CSPin, OUTPUT);
  SPI.begin();
}

void loop() {
  for (int i = 0; i <= maxPotiSteps; i++) {
    setPotValue(i);
  }
  delay(1000);
  for (int i = maxPotiSteps; i >= 0; i--) {
    setPotValue(i);
  }
}

void setPotValue(int value) {
  digitalWrite(CSPin, LOW);
  delay(20);
  SPI.transfer(address);
  SPI.transfer(value);
  delay(20);
  digitalWrite(CSPin, HIGH);
}