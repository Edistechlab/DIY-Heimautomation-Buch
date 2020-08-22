/*
Project:  Si7021 Temperatur und Luftfeuchte Sensor mit Heizung
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 19.08.2020
Version:  V1.0
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V2.7.1 
 - Board: esp32   by Espressif Systems   V1.0.2

Required libraries (sketch -> include library -> manage libraries)
 - Adafruit_Si7021 library V1.3.0 
 - Adafruit Unified Sensor Library V1.1.2

Wirering for the BME280 Sensor:
Si7021      ESP32     ESP8266     UNO       Mega
VCC         3.3V      3.3V        3.3V      3.3V      
GND         GND       GND         GND       GND
SCL         D1 / 5    22          A5        21
SDA         D2 / 4    21          A4        20
*/

#include "Adafruit_Si7021.h"
float temp = 0.00;
float hum = 0.00;
Adafruit_Si7021 sensor = Adafruit_Si7021();

void setup() {
  Serial.begin(115200);
  if (!sensor.begin()) {
    Serial.println("Si7021 Sensor nicht gefunden, überprüfe die Verkabelung!");
    while (true);
  }
}

void loop() {
  temp = sensor.readTemperature();
  hum = sensor.readHumidity();
  Serial.print("Humidity: ");
  Serial.print( String(hum));
  Serial.print("\tTemperature: ");
  Serial.print( String(temp));
  if (hum > 75) {
    sensor.heater(true);  //Heizung EIN
    Serial.println("\tHeizung ein - Temp korregiert");
    temp = temp - 1.8; // Korrektur um -1.8C wenn die Heizung an ist
  }
  else {
    sensor.heater(false);  // Heizung AUS
    Serial.println("\tHeizung aus");
  }
  delay(1000);
}
