/*
Project:  DS18B20 Temperatur Sensor on 1-Wire - read address
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 18.10.2021
Version:  V1.0
IDE:      Arduino IDE 1.8.16
 
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

const int oneWireBus = 4;     
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);
DeviceAddress sensor_address; 

void setup(){
  Serial.begin(115200);
  sensors.begin();
  sensors.getAddress(sensor_address, 0);
  Serial.print("Sensor Adresse: ");
  printAddress(sensor_address);
  Serial.println();
}

void loop(){ 
  sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);   
  Serial.print(temperatureC);
  Serial.println("ºC");
  delay(5000);
}

void printAddress(DeviceAddress deviceAddress){
  for (uint8_t i = 0; i < 8; i++) {
    Serial.print("0x");
    if (deviceAddress[i] < 0x10) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
    if (i < 7) Serial.print(", ");
  }
  Serial.println("");
}
