/*
Project:  BH1750 Lux Sensor
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 03.08.2022 
Version:  V1.0
IDE:      Arduino IDE 1.8.19

Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V3.0.2 

Required libraries (Tools -> manage libraries)
 - BH1750 library V1.3.0 by Christopher Laws 
*/ 

#include <BH1750.h>
#include <Wire.h>

BH1750 lightMeter;

void setup(){
  Serial.begin(115200);
  Wire.begin();   //Wire.begin(SDA, SCL) wenn nicht die Standard Pins verwendet werden.
  lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x5C, &Wire);
}

void loop() {
  float lux = lightMeter.readLightLevel();
  Serial.print(" Lichtmenge: ");
  Serial.print(lux);
  Serial.println(" lx");
  delay(1000);
}
