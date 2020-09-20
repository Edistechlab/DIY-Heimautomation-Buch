/*
Project:  BME280 Temperatur und Luftfeuchte Sensor über SPI
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 29.08.2020
Version:  V1.0
IDE:      Arduino IDE 1.8.13
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V2.7.4 
 - Board: esp32   by Espressif Systems   V1.0.2

Required libraries (sketch -> include library -> manage libraries)
 - Adafruit BME280 library V2.1.0 

Wirering for the BME280 Sensor:
BME280      ESP32       ESP8266     UNO         Mega
VCC         3.3V        3.3V        3.3V        3.3V      
GND         GND         GND         GND         GND
SCL         D30/18      D5/14       13          52
SDA         D37/23      D7/13       11          50
CSB         D29/5       D8/15       10          53
SDO         D31/19      D6/12       12          51
*/

#include <Adafruit_BME280.h>
//NodeMCU ESP32
#define BME_SCK 18   // Serial Clock 
#define BME_MOSI 23  // Serial Data In 
#define BME_CS 5     // Chip Select 
#define BME_MISO 19  // Serial Data Out 
Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

float temp = 0.00;
float hum = 0.00;
float pressure = 0.00;

void setup() {
  Serial.begin(115200);
  if (!bme.begin()) {     
    Serial.println("BME280 Sensor nicht gefunden, überprüfe die Verkabelung oder Adresse!");
    while (true);
  }
}

void loop() {
  temp = bme.readTemperature();
  hum = bme.readHumidity();
  pressure = bme.readPressure()/100;  //von Pa auf hPa umrechnen
  Serial.print("Humidity: ");
  Serial.print( String(hum));
  Serial.print("\tTemperature: ");
  Serial.print( String(temp));
  Serial.print("\tPressure: ");
  Serial.println( String(pressure));
  delay(2000);    
}
