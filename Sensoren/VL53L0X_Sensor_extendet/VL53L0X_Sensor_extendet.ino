 /*
Project:  VL53L0X Laser Sensor Extendet
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 06.01.2025
Update:   
Version:  V1.0
IDE:      Arduino IDE 2.3.4

Required Board (Tools -> Board -> Boards Manager...)
- Microcontroller: Arduino Nano ESP32 
- Board: Arduino ESP32 Boards by Arduino V2.0.18.arduino5

Required libraries (Tools -> manage libraries)
 - VL53L0X by Pololu V1.3.1
*/

#include <VL53L0X.h>

// Sensor-Instanz erstellen
VL53L0X sensor;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Sensor initialisieren
  if (!sensor.init()) {
    Serial.println(F("VL53L0X nicht gefunden!"));
    while (1);
  }

  // Long-Range-Modus aktivieren
  sensor.setSignalRateLimit(0.1);                                 // Standard: 0.25 (reduzieren für höhere Empfindlichkeit)
  sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);   // Standard: 14
  sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14); // Standard: 10
  sensor.setMeasurementTimingBudget(33000);                       // Timing-Budget auf 33 ms erhöhen (für größere Reichweite)
}

void loop() {
  uint16_t distance = sensor.readRangeSingleMillimeters();

  if (sensor.timeoutOccurred()) {
    Serial.println(F("Zeitüberschreitung bei Messung!"));
  } else {
    Serial.print(F("Entfernung: "));
    Serial.print(distance);
    Serial.println(F(" mm"));
  }

  delay(100); // Kurze Pause zwischen Messungen
}
