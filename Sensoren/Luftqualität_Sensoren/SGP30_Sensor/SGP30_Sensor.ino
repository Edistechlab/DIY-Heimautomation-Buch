/*
Project:  SGP30 VOC I2C-Sensor / Modification from the Adafruit sgp30test code
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 28.11.2021
Version:  V1.0
IDE:      Arduino IDE 1.8.16
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V3.0.2 
 - Board: esp32   by Espressif Systems   V1.0.6

Required libraries (sketch -> include library -> manage libraries)
 - Adafruit_SGP30 library V2.0.0      
*/

#include <Wire.h>
#include "Adafruit_SGP30.h"
Adafruit_SGP30 sgp;
int counter = 0;
const int sensorTakt = 1000; //alle Sekunden wird der Sensor ausgelesen

uint32_t getAbsoluteHumidity(float temperature, float humidity) {
    // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
    const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
    const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity); // [mg/m^3]
    return absoluteHumidityScaled;
}

void setup() {
  Serial.begin(115200);

  if (! sgp.begin()){
    Serial.println("Sensor not found :-(");
    while (1);
  }
  //Hier können die Baseline Werte wenn vorhanden eingegeben werden, wenn nicht, diese Zeile auskommentierern
  //sgp.setIAQBaseline(0x7E94, 0x8A51);  // Baseline Werte nach 12h Betrieb vom Serial Monitor entnehmen!
}

void loop() {
  //Hier können die Luftfeuchte und Temperaturwerte für genauere Messwerte eingegeben werden.
  //float temperature = 22.1; // [°C]
  //float humidity = 45.2; // [%RH]
  //sgp.setHumidity(getAbsoluteHumidity(temperature, humidity));

  if (! sgp.IAQmeasure()) {
    Serial.println("Measurement failed");
    return;
  }
  Serial.print("TVOC "); Serial.print(sgp.TVOC); Serial.print(" ppb\t");
  Serial.print("eCO2 "); Serial.print(sgp.eCO2); Serial.println(" ppm");
 
  delay(sensorTakt);

  counter++;
  if (counter == 30) {
    counter = 0;
    uint16_t TVOC_base, eCO2_base;
    if (! sgp.getIAQBaseline(&eCO2_base, &TVOC_base)) {
      Serial.println("Failed to get baseline readings");
      return;
    }
    Serial.print("Baseline Werte: eCO2: 0x"); Serial.print(eCO2_base, HEX);
    Serial.print(" & TVOC: 0x"); Serial.println(TVOC_base, HEX);
  }
}
