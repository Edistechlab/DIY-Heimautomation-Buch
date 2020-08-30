/*
Project:  BME280 Temperatur und Luftfeuchte Sensor mit I2C
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
SCL         22          D1/5        A5          21
SDA         21          D2/4        A4          20
*/

#include <Adafruit_BME280.h>
Adafruit_BME280 bme;
float temp = 0.00;
float hum = 0.00;
float pressure = 0.00;

void setup() {
  Serial.begin(115200);
  if (!bme.begin(0x76, &Wire)) {     //I2C Adresse ist 0x76 oder 0x77
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
