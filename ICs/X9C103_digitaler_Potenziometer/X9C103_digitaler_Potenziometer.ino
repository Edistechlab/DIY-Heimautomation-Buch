/*
Project:  Digitaler Potenziometer X9C103
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 04.11.2023
Version:  V1.0
IDE:      Arduino IDE 2.1.1
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: Arduino ESP32 Boards - by Arduino   V2.0.13

Required libraries (sketch -> include library -> manage libraries)
 - Bounce2 by Thomas O Fredericks V2.71
*/

#include <Bounce2.h> 

const byte buttonPinSAVE = 6;
const byte buttonPinDOWN = 7;
const byte buttonPinUP = 8;
const byte CS_Pin = 9;
const byte UD_Pin = 10;
const byte INC_Pin = 11;
const byte debounceInterval = 10; // Zeit in (ms) für depounce

Bounce2::Button buttonSAVE = Bounce2::Button();
Bounce2::Button buttonDOWN = Bounce2::Button();
Bounce2::Button buttonUP = Bounce2::Button();

void setup() {
  pinMode(buttonPinSAVE, INPUT_PULLUP);
  pinMode(buttonPinUP, INPUT_PULLUP);
  pinMode(buttonPinDOWN, INPUT_PULLUP);

  pinMode(CS_Pin, OUTPUT);
  pinMode(INC_Pin, OUTPUT);
  pinMode(UD_Pin, OUTPUT);

  buttonSAVE.attach(buttonPinSAVE);
  buttonSAVE.interval(debounceInterval);
  buttonUP.attach(buttonPinUP);
  buttonUP.interval(debounceInterval); 
  buttonDOWN.attach(buttonPinDOWN);
  buttonDOWN.interval(debounceInterval); 
}

void loop() {
  buttonSAVE.update();
  buttonUP.update();
  buttonDOWN.update();
 
  if (buttonSAVE.fell()) {saveValue();}
  if (buttonUP.fell()) {countUp();}
  if (buttonDOWN.fell()) {countDown();}
}

void countUp() {
  digitalWrite(UD_Pin, HIGH);
  digitalWrite(CS_Pin, LOW);
  delay(5);
  digitalWrite(INC_Pin, HIGH);
  delay(5);
  digitalWrite(INC_Pin, LOW);
  delay(5);
}

void countDown() {
  digitalWrite(UD_Pin, LOW);
  digitalWrite(CS_Pin, LOW);
  digitalWrite(INC_Pin, HIGH);
  delay(5);
  digitalWrite(INC_Pin, LOW);
  delay(5);
} 

//Speichern sollte laut Datenblatt funktionieren, ist aber ungetestet!
void saveValue() {
  digitalWrite(CS_Pin, LOW);
  digitalWrite(INC_Pin, HIGH);
  delay(5);
  digitalWrite(CS_Pin, HIGH);
  delay(20);  // 20 Millisek. für das Speichern
  digitalWrite(INC_Pin, LOW);
  delay(5);
}
