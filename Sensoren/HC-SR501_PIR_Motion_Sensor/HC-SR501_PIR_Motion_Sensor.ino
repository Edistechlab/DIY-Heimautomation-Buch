/*
Project:  PIR Motion dedector HC-SR501 with an ESP8266/NodeMCU
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 10.03.2021 
Version:  V1.0
IDE:      Arduino IDE 1.8.13

Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V2.7.4 

Wirering for the Motion Sensor:
HC-SR501    NodeMCU
VCC         Vin (5V)
GND         GND
OUT         D1 / GPIO5
*/
 
int motionPin = 5;
int pirState = LOW;
int motion = 0;
 
void setup() {
  pinMode(motionPin, INPUT);     
  Serial.begin(115200);
}
 
void loop(){
  motion = digitalRead(motionPin);
  if (motion == HIGH) {
    if (pirState == LOW) {
      Serial.println("Bewegung erkannt");
      pirState = HIGH;
    }
  } else {
    if (pirState == HIGH){
      Serial.println("Keine Bewegung");
      pirState = LOW;
    }
  }
}
