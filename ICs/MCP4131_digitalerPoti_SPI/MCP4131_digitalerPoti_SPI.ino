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

const byte CSPin = 4;
const byte analogInPin = 17;  //A0
const byte maxPotiSteps = 128;  // 0 - 128 or 0 - 255 Steps
byte address = 0x00;

void setup() {
  Serial.begin(115200);
  pinMode (CSPin, OUTPUT);
  pinMode (analogInPin, INPUT);
  SPI.begin();
}

void loop() {
  setPotValue(5);
  readPotValue();
  delay(1000);
  setPotValue(80);
  readPotValue();
  delay(1000);
  setPotValue(maxPotiSteps);
  readPotValue();
  delay(1000);
  setPotValue(50);
  readPotValue();
  delay(1000);
}

void setPotValue(int value) {
  digitalWrite(CSPin, LOW);
  delay(20);
  SPI.transfer(address);
  SPI.transfer(value);
  delay(100);
  digitalWrite(CSPin, HIGH);
}

void readPotValue() {
  int sensorValue = analogRead(analogInPin);
  Serial.println(sensorValue);
  delay(100);
}
  



