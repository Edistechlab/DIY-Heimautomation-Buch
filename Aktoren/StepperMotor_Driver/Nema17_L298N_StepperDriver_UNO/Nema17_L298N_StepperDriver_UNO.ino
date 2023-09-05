/*
Project:  Arduino UNO steuert einen NEMA17 Motor über das L298N Modul
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 03.09.2023
Version:  V1.0
IDE:      Arduino IDE 2.1.1

Required libraries (sketch -> include library -> manage libraries)
 - AccelStepper by Mike McCauley V1.64
*/

#include <AccelStepper.h>
#define MotorInterfaceType 4
AccelStepper stepper = AccelStepper(MotorInterfaceType, 8, 9, 10, 11);

void setup() {
  stepper.setMaxSpeed(800);  // Maximale Geschwindigkeit
  stepper.setAcceleration(100);  // Beschleunigung Schritte pro sek.^2 
}

void loop() {
  stepper.moveTo(1000);  // Zielposition
  stepper.runToPosition();
  delay(1000);
  stepper.moveTo(0);  // Anfangsposition
  stepper.runToPosition();
  delay(1000);
}