/*
Project:  BME680 Temperatur/Luftfeuchte/Luftdruck/Gas Sensor mit SPI
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
SCL         D30/18      D5/14       13          52
SDA         D37/23      D7/13       11          50
SDO         D31/19      D6/12       12          51
CS          D29/5       D8/15       10          53
*/

#include "Adafruit_BME680.h"
//NodeMCU ESP32
#define BME_SCK 18   // Serial Clock 
#define BME_MOSI 23  // Serial Data In 
#define BME_CS 5     // Chip Select 
#define BME_MISO 19  // Serial Data Out 
Adafruit_BME680 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK);

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
