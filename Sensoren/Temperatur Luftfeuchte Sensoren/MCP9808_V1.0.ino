/*
Project:  MCP9808 Temperatur Sensor
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 23.08.2020
Version:  V1.0
IDE:      Arduino IDE 1.8.13
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V2.7.4 
 - Board: esp32   by Espressif Systems   V1.0.2

Required libraries (sketch -> include library -> manage libraries)
 - Adafruit_MCP9808 library V1.1.2 

Wirering for the MCP9808 Sensor:
MCP9808     ESP32       ESP8266     UNO         Mega
VCC         3.3V        3.3V        3.3V        3.3V      
GND         GND         GND         GND         GND
SCL         D1/5        22          A5          21
SDA         D2/4        21          A4          20
A0          3.3V/GND    3.3V/GND    3.3V/GND    3.3V/GND  
A1          3.3V/GND    3.3V/GND    3.3V/GND    3.3V/GND
A2          3.3V/GND    3.3V/GND    3.3V/GND    3.3V/GND  
ALERT       input Pin   input Pin   input Pin   input Pin
*/

#include "Adafruit_MCP9808.h"

float temp = 0.00;
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();

void setup() {
  Serial.begin(115200);
  if (!tempsensor.begin(0x18)) {
    Serial.println("MCP9808 Sensor nicht gefunden, überprüfe die Adresse oder Verkabelung!");
    while (true);
  }
  tempsensor.setResolution(3);  // 0.0625°C    250 ms
}

void loop() {
  tempsensor.wake();   // wake up the Sensor
  temp = tempsensor.readTempC();
  Serial.print("Temperature: ");
  Serial.print( String(temp));
  Serial.println("°C");
  delay(2000);
  tempsensor.shutdown_wake(1); // shutdown the sensor; power consumption ~0.1 mikro Ampere
  delay(200);
}
