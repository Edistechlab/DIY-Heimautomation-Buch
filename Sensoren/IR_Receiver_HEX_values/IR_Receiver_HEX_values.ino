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
IRrecv receiver(ReceiverPin);
decode_results results;

void setup() {
  Serial.begin(115200);
  receiver.enableIRIn();
}

void loop() {
  if (receiver.decode(&results)) {
    Serial.println(results.value, HEX);
    receiver.resume(); 
  }
}
