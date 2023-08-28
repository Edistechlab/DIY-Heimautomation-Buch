/*
Project:  2x 28BYJ-48 Steppermotor with the UDL2003 Modul - Arduino UNO
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 28.08.2023
Version:  V1.0
IDE:      Arduino IDE 2.1.1

Required libraries (sketch -> include library -> manage libraries)
 - AccelStepper by Mike McCauley V1.64

Wirering the UDL2003 Moduls:  
IN1         11      6
IN2         10      5   
IN3         9       4
IN4         8       3
5 Volt and GND on + / -
*/

#include <AccelStepper.h>
const byte Fullstep = 4;
const byte Halfstep = 8;
const short fullRevolution = 2038;

// Pins IN1-IN3-IN2-IN4
AccelStepper stepper1(Halfstep, 11, 9, 10, 8);
AccelStepper stepper2(Fullstep, 6, 4, 5, 3);

void setup(void) {
  Serial.begin(115200);
  stepper1.setMaxSpeed(1000.0);
  stepper1.setAcceleration(50);
  stepper1.setSpeed(200);
  stepper1.moveTo(fullRevolution);

  stepper2.setMaxSpeed(1000.0);
  stepper2.setAcceleration(50);
  stepper2.setSpeed(200);
  stepper2.moveTo(-fullRevolution); 
}

void loop(void) {
  if (stepper1.distanceToGo() == 0) 
    stepper1.moveTo(-stepper1.currentPosition());
  if (stepper2.distanceToGo() == 0) 
    stepper2.moveTo(-stepper2.currentPosition());
  
  stepper1.run();
  stepper2.run();
}