/*
Project:  BME680 Temperatur/Luftfeuchte/Luftdruck/Gas Sensor mit I2C
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 11.02.2021
Version:  V1.0
IDE:      Arduino IDE 1.8.13
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V2.7.4 
 - Board: esp32   by Espressif Systems   V1.0.4

Required libraries (sketch -> include library -> manage libraries)
 - Adafruit BME680 library V1.1.1 

Wirering for the BME680 Sensor:
BME680      ESP32       ESP8266     UNO         Mega
VCC         3.3V        3.3V        3.3V        3.3V      
GND         GND         GND         GND         GND
SCL         22          D1/5        A5          21
SDA         21          D2/4        A4          20
*/

#include "Adafruit_BME680.h"
Adafruit_BME680 bme; // I2C

float temp = 0.00;
float hum = 0.00;
float druck = 0.00;
float gas = 0.00;

void setup() {
  Serial.begin(115200);
  if (!bme.begin()) {
    Serial.println("BME680 Sensor nicht gefunden, überprüfe die Verkabelung oder Adresse!");
    while (true);
  }

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
}

void loop() {
  temp = bme.readTemperature();
  hum = bme.readHumidity();
  druck = bme.readPressure() /100;    //von Pa auf hPa umrechnen
  gas = bme.gas_resistance / 1000;    //von Ohm auf kOhm umrechnen
  
  Serial.print("Humidity: ");
  Serial.print( String(hum));
  Serial.println(" %");
  Serial.print("Temperature: ");
  Serial.print( String(temp));
  Serial.println(" *C");
  Serial.print("Pressure: ");
  Serial.print( String(druck));
  Serial.println(" hPa");
  Serial.print("Gas: ");
  Serial.print( String(gas));
  Serial.println(" KOhms");
  delay(3000);    
}
