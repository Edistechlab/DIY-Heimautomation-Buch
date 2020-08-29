/*
Project:  DHT11 Temperatur und Luftfeuchte Sensor
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 29.08.2020
Version:  V1.0
IDE:      Arduino IDE 1.8.13
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V2.7.4 
 - Board: esp32   by Espressif Systems   V1.0.2

Required libraries (sketch -> include library -> manage libraries)
 - Adafruit_DHT sensor library V1.3.10 
 - Adafruit Unified Sensor Library V1.1.4

Wirering for the DHT11 Sensor:
DHT11       ESP32       ESP8266     UNO         Mega
VCC         3.3V        3.3V        3.3V        3.3V      
GND         GND         GND         GND         GND
OUT         22          D1/5        A5          21
*/

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#define DHTPIN 5        //Pin an dem das OUT Signal angeschlossen ist.
float temp = 0.00;
float hum = 0.00;
DHT_Unified dht(DHTPIN, DHT11);  

void setup() {
  Serial.begin(115200);
  dht.begin();
}

void loop() {
  sensors_event_t event;
  dht.humidity().getEvent(&event);    //Luftfeuchte messen und ausgeben
  hum = event.relative_humidity;
  Serial.print("Humidity: ");
  Serial.print( String(hum));
  dht.temperature().getEvent(&event); //Temperatur messen und ausgeben
  temp = event.temperature;
  Serial.print("\tTemperature: ");
  Serial.println( String(temp));
  delay(1000);    // eine Sekunden warten, DHT11 Sensor Antwortzeit ist 1 Sek.
}
