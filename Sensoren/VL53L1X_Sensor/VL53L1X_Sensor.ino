 /*
Project:  VL53L1X Laser Sensor
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 23.01.2025
Update:   
Version:  V1.0
IDE:      Arduino IDE 2.3.4

Required Board (Tools -> Board -> Boards Manager...)
- Microcontroller: Wemos d1 mini
- Board: ESP8266 by ESP8266 Community V3.1.2

Required libraries (Tools -> manage libraries)
 - VL53L1X by Pololu V1.3.1
*/

#include <VL53L1X.h>

// Sensor-Instanz erstellen
VL53L1X sensor;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  sensor.setTimeout(500);

  // Sensor initialisieren
  if (!sensor.init()) {
    Serial.println(F("VL53L1X nicht gefunden!"));
    while (1);
  }
  sensor.setDistanceMode(VL53L1X::Long);
  sensor.setMeasurementTimingBudget(50000);
  sensor.startContinuous(50);
}

void loop() {
  // Entfernung messen
  uint16_t distance = sensor.read();

  if (sensor.timeoutOccurred()) {
    Serial.println(F("Zeitüberschreitung bei Messung!"));
  } else {
    Serial.print(F("Entfernung: "));
    Serial.print(distance);
    Serial.println(F(" mm"));
  }
}