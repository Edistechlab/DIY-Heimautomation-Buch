/*
Project:  Arduino UNO steuert einen NEMA17 Motor über das DRV8825 Modul
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 06.09.2023
Version:  V1.0
IDE:      Arduino IDE 2.1.1

Required libraries (sketch -> include library -> manage libraries)
 - AccelStepper by Mike McCauley V1.64
*/
#include <AccelStepper.h>
#define motorInterfaceType 1
AccelStepper stepper(motorInterfaceType, 3, 2);  //STEP-Pin, DIR-Pin

void setup() {
  stepper.setMaxSpeed(500);  // Maximale Geschwindigkeit
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