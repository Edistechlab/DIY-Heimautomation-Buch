/*
Project:  L9110 H-Brücke mit einem 12 Volt Motor
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 08.07.2024
Version:  V1.0
IDE:      Arduino IDE 2.3.2
Board:    Wemos d1 mini light
Pins:     IA=D6 / IB=D5

Required Board (Tools -> Board -> Boards Manager...)
- Board: esp8266 by ESP8266 Community   V3.1.2 

Required libraries (sketch -> include library -> manage libraries)
 -
*/

const uint8_t Motor_IA = 12;  //D6
const uint8_t Motor_IB = 14;  //D5
const uint8_t speed = 150;    // 0 - 255

void setup() {
  Serial.begin(115200);
  pinMode( Motor_IA, OUTPUT );
  pinMode( Motor_IB, OUTPUT );
}

void loop() {
  digitalWrite(Motor_IA,LOW);
  digitalWrite(Motor_IB,HIGH);
  delay(2000);
  digitalWrite(Motor_IA,LOW);
  analogWrite(Motor_IB,speed);
  delay(2000);
  digitalWrite(Motor_IA,HIGH);
  digitalWrite(Motor_IB,LOW);
  delay(2000);
  digitalWrite(Motor_IA,HIGH);
  analogWrite(Motor_IB,speed);
  delay(2000);
}