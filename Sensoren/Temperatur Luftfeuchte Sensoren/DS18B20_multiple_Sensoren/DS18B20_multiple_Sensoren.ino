/*
Project:  Multiple DS18B20 Temperatur Sensors on 1-Wire
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 17.10.2021
Version:  V1.0
IDE:      Arduino IDE 1.8.16
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V3.0.2 
 - Board: esp32   by Espressif Systems   V1.0.6

Required libraries (sketch -> include library -> manage libraries)
 - OneWire libary V2.3.5 by Jim Studt
 - DallasTemperature V3.9.0 by Miles Burton

Connect the DS18B20 Sensor:
Sensor         ESP32    ESP8266    UNO      Mega
VDD            3,3V     3,3V       5V       5V
DQ / 4,7kOhm   4        4          4        4
GND            GND      GND        GND      GND             
*/

#include <OneWire.h>
#include <DallasTemperature.h>

const int oneWireBus = 4;   //GPIO Pin 1-Wire Bus
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

//Sensor 1: 28  CC  32  07  D6  01  3C  6C
//Sensor 2: 28  F7  2C  07  D6  01  3C  A7
float tempSensor1, tempSensor2;
uint8_t sensor1[8] = { 0x28, 0xCC, 0x32, 0x07, 0xD6, 0x01, 0x3C, 0x6C  };
uint8_t sensor2[8] = { 0x28, 0xF7, 0x2C, 0x07, 0xD6, 0x01, 0x3C, 0xA7  };

void setup() {
  Serial.begin(115200);
  sensors.begin();
}

void loop() {
  sensors.requestTemperatures(); 
  tempSensor1 = sensors.getTempC(sensor1); // Gets the Sensor 1 values
  tempSensor2 = sensors.getTempC(sensor2); // Gets the Sensor 2 values
  Serial.print(tempSensor1);
  Serial.println(" ºC  - Sensor 1");
  Serial.print(tempSensor2);
  Serial.println(" ºC  - Sensor 2");  
  delay(5000);
}
