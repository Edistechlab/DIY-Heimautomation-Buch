/*
Project:  28BYJ-48 Steppermotor with the UDL2003 Modul - Arduino UNO and 2 buttons
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 27.08.2023
Version:  V1.0
IDE:      Arduino IDE 2.1.1

Required libraries (sketch -> include library -> manage libraries)
 - AccelStepper by Mike McCauley V1.64
 - Bounce2 by Thomas O Fredericks V2.71 

Wirering for the UDL2003 Modul:  
IN1         11    
IN2         10       
IN3         9    
IN4         8
5 Volt and GND on + / -

Button 1 is on GND and Pin 5
Button 2 is on GND and Pin 4
*/

#include <AccelStepper.h>
#include <Bounce2.h>
const byte btnBack = 5;
const byte btnForward = 4;
const byte Fullstep = 4;
const byte Halfstep = 8;
const short fullRevolution = 2038;
const float SteppDegree = 11.32; // Halfstep 11.32 - Fullstep 5.66
float degree = 20;  // pro Tastendruck
float moveRev = degree * SteppDegree;
float Move = 0;
Bounce2::Button buttonBack = Bounce2::Button();
Bounce2::Button buttonForw = Bounce2::Button();

// Pins IN1-IN3-IN2-IN4
AccelStepper stepper1(Halfstep, 11, 9, 10, 8);

void setup(void) {
  Serial.begin(115200);
  buttonBack.attach(btnBack, INPUT_PULLUP);
  buttonBack.interval(30); // 30 ms zum Entprellen
  buttonBack.setPressedState(LOW);  // Input = LOW bei gedrückten Taster
  buttonForw.attach(btnForward, INPUT_PULLUP);
  buttonForw.interval(30); // 30 ms zum Entprellen
  buttonForw.setPressedState(LOW);  // Input = LOW bei gedrückten Taster
  stepper1.setMaxSpeed(1000.0);   // set the maximum speed
  stepper1.setAcceleration(50); // set acceleration
  stepper1.setSpeed(200);         // set initial speed
  stepper1.setCurrentPosition(0); // set position
}

void loop(void) {
  buttonBack.update();
  buttonForw.update();
  if ( buttonBack.pressed() ) {
    //Serial.println("Motor rückwärts");
    Move = Move + moveRev;
    Serial.print("Motor rückwärts auf Pos.: "); Serial.println(Move);
  }
  if ( buttonForw.pressed() ) {
    Move = Move - moveRev;
    Serial.print("Motor vorwärts auf Pos.: "); Serial.println(Move);
  }
  stepper1.moveTo(Move);
  stepper1.run();
}
