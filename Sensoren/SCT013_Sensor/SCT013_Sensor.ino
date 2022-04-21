 /*
Project:  SCT-013 Sensor - Power meassurement
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 20.04.2022 
Version:  V1.0
IDE:      Arduino IDE 1.8.19

Required Board (Tools -> Board -> Boards Manager...)
 - Board: ESP8266 by ESP8266 Community   V3.0.2
          ESP32 by Espressif Systems V1.0.6

Required libraries (Tools -> manage libraries)
 - EmonLib libary V1.1.0 by OpenEnergyMonitor

Based on EmonLibrary examples openenergymonitor.org, Licence GNU GPL V3
*/ 

#include "EmonLib.h"
EnergyMonitor emon1;
const byte current1Pin = A0; // ADC-PIN 
const byte Spannung = 230;  // Spannung von eurem Netz

void setup() {  
  Serial.begin(115200);
  //analogReadResolution(ADC_BITS); // 12 Bit Auflösung aktivieren
  emon1.current(current1Pin, 18.8);  // Pin und Kalibrierung
}

void loop() {
  double Irms = emon1.calcIrms(1480);  
  Serial.print(Irms*Spannung);
  Serial.print(" Watt  -  ");
  Serial.print(Irms);
  Serial.println(" Ampere");
  delay(1000);
}
