/*
Project:  LDR - Fotowiderstand Test
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 25.06.2020
Version:  V1.0
IDE:      Arduino IDE 1.8.13

Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V2.7.4 
*/

const int ledPin=14;
const int ldrPin=A0;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(ldrPin, INPUT);
}

void loop() {
  int value = analogRead(A0);
  Serial.println("Analog Value :");
  Serial.println(value);
  if (value < 500) {
    digitalWrite(ledPin, HIGH);
  }
  else {
    digitalWrite(ledPin, LOW);
  }
  delay(200);
}
