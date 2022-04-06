/*
Project:  IR Daten empfangen und zwei LEDs schalten
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 03.04.2022 
Version:  V1.0
IDE:      Arduino IDE 1.8.19

Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V3.0.2 

Required libraries (Tools -> manage libraries)
 - Irremote by Armin Joachimsmeyer V3.6.1

Drei Taster auf der Fernbedienung
Protocol=NEC Address=0xDF00 Command=0x47 Raw-Data=0xB847DF00 32 bits LSB first - links
Protocol=NEC Address=0xDF00 Command=0x7 Raw-Data=0xF807DF00 32 bits LSB first - rechts
Protocol=NEC Address=0xDF00 Command=0x6 Raw-Data=0xF906DF00 32 bits LSB first - OK
*/ 

#include <IRremote.h>
const int ReceiverPin = 5; // Pin D1 beim Wemos D1 mini
const int LEDrotPin = 4; // Pin D2 beim Wemos D1 mini
const int LEDgelbPin = 2; // Pin D4 beim Wemos D1 mini

void setup() {
  Serial.begin(115200);
  pinMode(LEDrotPin, OUTPUT);
  pinMode(LEDgelbPin, OUTPUT);
  IrReceiver.begin(ReceiverPin, DISABLE_LED_FEEDBACK);
  printActiveIRProtocols(&Serial);
}

void loop() {
  if (IrReceiver.decode()) {
    IrReceiver.printIRResultShort(&Serial);
    Serial.println();   
    if (IrReceiver.decodedIRData.address == 0xDF00) {    
      if (IrReceiver.decodedIRData.command == 0x47) {
         digitalWrite(LEDrotPin, HIGH);      
      }
      if (IrReceiver.decodedIRData.command == 0x7) {
         digitalWrite(LEDgelbPin, HIGH);      
      }
      if (IrReceiver.decodedIRData.command == 0x6) {
         digitalWrite(LEDrotPin, LOW);
         digitalWrite(LEDgelbPin, LOW);
      }
    }
    IrReceiver.resume();
  }
}
