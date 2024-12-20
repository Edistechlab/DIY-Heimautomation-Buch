/*
Project:  JSN-SR04T Mode1: Automatic Serial Mode with Wemos d1 mini light
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 19.12.2024
Version:  V1.0 
IDE:      Arduino IDE 2.3.4

Required Board (Tools -> Board -> Boards Manager...)
ESP:      Wemos d1 mini light
  - Board: esp8266 by ESP8266 Community V3.1.2

Required libraries (sketch -> include library -> manage libraries)
 - 
*/

#include <SoftwareSerial.h>

// Pin definition
const uint8_t RXPin = 12; //D6
const uint8_t TXPin = 13; //D7

// Variables
unsigned char buffer[4] = {0};
uint16_t distance = 0;

SoftwareSerial mySerial(RXPin, TXPin);

void setup() {
  Serial.begin(115200);
  mySerial.begin(9600);
}

void loop() {
  unsigned long timeout = millis() + 1000;

  if(mySerial.available()) {
    if (mySerial.read() == 0xff) {
      buffer[0] = 0xff;

      for (uint8_t i = 1; i < 4; i++) {        
        if (millis() > timeout) { // Timeout check
          Serial.println("Timeout reached! No complete data received.");
          return;
        }        
        if (mySerial.available()) {
          buffer[i] = mySerial.read();
        }
      }

      distance = (buffer[1]<<8) + buffer[2]; //combine the two 8-Bit Values
      
      if(((buffer[0] + buffer[1] + buffer[2]) & 0xFF) != buffer[3]) { // check if the Checksum is ok
        Serial.println("Invalid result");
      } else {
        Serial.print("Distance in mm: "); 
        Serial.println(distance);
      } 
    }
  }   
}
