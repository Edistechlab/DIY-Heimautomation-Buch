/*
Project:  DS18B20 Temperatur Sensor on 1-Wire
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 17.10.2021
Version:  V1.0
IDE:      Arduino IDE 1.8.15
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V3.0.2 
 - Board: esp32   by Espressif Systems   V1.0.6

Required libraries (sketch -> include library -> manage libraries)
 - OneWire libary V2.3.5 by Jim Studt
 - DallasTemperature V3.9.0 by Miles Burton

Connect the DS18B20 Sensor:
Sensor          ESP32    ESP8266    UNO      Mega
VDD             3,3V     3,3V       5V       5V
DQ / 4,7kOhm    4        4          4        4
GND             GND      GND        GND      GND             
*/

#include <OneWire.h>
#include <DallasTemperature.h>

const int oneWireBus = 4;   //GPIO Pin 1-Wire Bus
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(115200);
  sensors.begin();
}

void loop() {
  sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);
  float temperatureF = sensors.getTempFByIndex(0);     
  Serial.print(temperatureC);
  Serial.println("ºC");
  Serial.print(temperatureF);
  Serial.println("ºF");
  delay(5000);
}
