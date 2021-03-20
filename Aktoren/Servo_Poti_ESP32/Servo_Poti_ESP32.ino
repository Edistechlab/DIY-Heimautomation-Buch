/*
Project:  SG90 Servo mit Poti am ESP32  
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 03.08.2020
Version:  V1.0
*/

#include <ESP32Servo.h>

Servo myservo;//Servo Objekt erstellen

const int servoPin = 32;  // GPIO pin 32 für den Servomotor
const int potPin = 33;    // ADC PIN 33 für Potenziometer
int val;

void setup(){
  myservo.setPeriodHertz(50);
  myservo.attach(servoPin);
}

void loop() {
  val = analogRead(potPin);
  val = map(val, 0, 4096, 0, 180);
  myservo.write(val);
  delay(500); 
}
