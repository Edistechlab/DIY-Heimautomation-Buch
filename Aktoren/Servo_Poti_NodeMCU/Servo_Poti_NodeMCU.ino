/*
Project:  SG90 Servo mit Poti am Arduino oder NodeMCU ESP8266  
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 03.08.2020
Version:  V1.0
*/

#include <Servo.h>

Servo myservo;    //Servo Objekt erstellen

const int servoPin = 2; //Pin 9 Uno; Pin 2(D4) Node MCU ESP8266
const int potpin = 0;   //Analog pin A0 für das Potenziometer
int val;

void setup() {
  Serial.begin(115200);
  myservo.attach(servoPin);
  }

void loop() {
  val = analogRead(potpin); // (Wert zwischen 0 und 1023)
  val = map(val, 0, 1023, 0, 180);
  myservo.write(val);
  delay(500);
}
