/*
Project:  IR Daten empfangen und die HEX Werte ausgeben
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 03.04.2022 
Version:  V1.0
IDE:      Arduino IDE 1.8.19

Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V3.0.2 

Required libraries (Tools -> manage libraries)
 - Irremote by Armin Joachimsmeyer V3.6.1
*/ 

#include <IRremote.h>
const int ReceiverPin = 5; // Pin D1 beim Wemos D1 mini

void setup() {
  Serial.begin(115200);
  IrReceiver.begin(ReceiverPin, DISABLE_LED_FEEDBACK);
  printActiveIRProtocols(&Serial);
}

void loop() {
  if (IrReceiver.decode()) {
    IrReceiver.printIRResultShort(&Serial);
    Serial.println();
    IrReceiver.resume();
  }
}
