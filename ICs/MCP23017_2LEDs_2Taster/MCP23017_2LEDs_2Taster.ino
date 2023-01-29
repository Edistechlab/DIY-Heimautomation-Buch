/*
Project:  MCP23017 - zwei LEDs und Taster
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 29.01.2023
Version:  V1.0
IDE:      Arduino IDE 2.0.3
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V3.1.1 

Required libraries (sketch -> include library -> manage libraries)
 - Adafruit MCP23017 library V2.2.0 / https://github.com/adafruit/Adafruit-MCP23017-Arduino-Library
 - Adafruit BusIO library V1.14.0 / https://github.com/adafruit/Adafruit_BusIO

Wirering the MCP23017 Modul:
MCP23017      ESP8266    
VCC         3.3V            
GND         GND        
SCL         D1/5       
SDA         D2/4   
*/

#include <Adafruit_MCP23X17.h>

const uint8_t LED0_Pin = 0;      // GPA0 (21) MCP23017
const uint8_t LED1_Pin = 1;      // GPA1 (22) MCP23017
const uint8_t Button0_Pin = 8;   // GPB0 (1) MCP23017
const uint8_t Button1_Pin = 9;   // GPB1 (2) MCP23017
#define MCP23017_I2C_addr0 0x20  // A0=GND, A1=GND, A2=GND 

Adafruit_MCP23X17 mcp0;

void setup() {
  Serial.begin(115200);
  if (!mcp0.begin_I2C(MCP23017_I2C_addr0)) {
      Serial.println("Error.");
    while (1);
  }
  mcp0.pinMode(Button0_Pin, INPUT_PULLUP);
  mcp0.pinMode(Button1_Pin, INPUT_PULLUP);
  mcp0.pinMode(LED0_Pin, OUTPUT);
  mcp0.pinMode(LED1_Pin, OUTPUT);
}

void loop() {
  // LOW = gedrückt, HIGH = nicht gedrückt
  mcp0.digitalWrite(LED0_Pin, !mcp0.digitalRead(Button0_Pin));
  mcp0.digitalWrite(LED1_Pin, !mcp0.digitalRead(Button1_Pin));
}