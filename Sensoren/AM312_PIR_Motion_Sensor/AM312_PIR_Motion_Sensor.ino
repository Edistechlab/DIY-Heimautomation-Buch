/*
Project:  PIR Motion dedector AM312 / ESP8266/NodeMCU
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 10.03.2021 
Version:  V1.0
IDE:      Arduino IDE 1.8.13

Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V2.7.4 

Wirering for the Motion Sensor:
AM312       NodeMCU
VCC         Vin (5V)
GND         GND
OUT         D1 / GPIO5
*/
 
int motionPin = 5;
int pirState = LOW;
 
void setup() {
  pinMode(motionPin, INPUT);     
  Serial.begin(115200);
}
 
void loop(){
  if (digitalRead(motionPin) == HIGH) {
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
