/*
Project:  IR Daten senden
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 03.04.2022 
Version:  V1.0
IDE:      Arduino IDE 1.8.19

Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V3.0.2 

Required libraries (Tools -> manage libraries)
 - Irremote by Armin Joachimsmeyer V3.6.1
*/ 

#include <Arduino.h>
#include <IRremote.hpp>

const int SenderPin = 5; // Pin D1 beim NodeMCU ESP8266
uint32_t rawData = 0xF906DF00;
uint16_t Address = 0xDF00;
uint8_t Command = 0x47;
uint8_t Repeats = 0;

void setup() {
  Serial.begin(115200);
  pinMode(SenderPin, OUTPUT);
  IrSender.begin(SenderPin, DISABLE_LED_FEEDBACK);
}

void loop() {
  IrSender.sendNEC(Address, Command, Repeats); //mit dem NEC Protokoll senden
  delay(500);
  IrSender.sendNECRaw(rawData, Repeats);  //Rohdaten mit dem NEC Protokoll senden
  delay(1000); // mindestens 5 ms Pause nach dem Senden
}
