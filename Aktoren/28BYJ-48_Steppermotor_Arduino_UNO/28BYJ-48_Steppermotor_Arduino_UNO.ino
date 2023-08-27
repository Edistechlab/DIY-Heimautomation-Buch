/*
Project:  28BYJ-48 Steppermotor with the UDL2003 Modul - Arduino UNO
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 15.08.2023
Version:  V1.0
IDE:      Arduino IDE 2.1.1

Required libraries (sketch -> include library -> manage libraries)
 - AccelStepper V1.64 by Mike McCauley

Wirering for the UDL2003 Modul:  
IN1         11    
IN2         10       
IN3         9    
IN4         8
5 Volt and GND on + / -
*/

#include <AccelStepper.h>
const byte Fullstep = 4;
const byte Halfstep = 8;
const short fullRevolution = 2038;
const float SteppDegree = 11.32; // Halfstep 11.32 - Fullstep 5.66

// Pins IN1-IN3-IN2-IN4
AccelStepper stepper1(Halfstep, 11, 9, 10, 8);

void setup(void) {
  stepper1.setMaxSpeed(1000.0);   // set the maximum speed
  stepper1.setAcceleration(50); // set acceleration
  stepper1.setSpeed(200);         // set initial speed
  stepper1.setCurrentPosition(0); // set position
}

void loop(void) {
  float degree = 45;  // im Uhrzeigersinn
  float moveRev = degree * SteppDegree;
  stepper1.moveTo(-moveRev);  // - für Uhrzeigersinn
  stepper1.run();
}
