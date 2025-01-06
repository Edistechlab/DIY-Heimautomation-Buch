 /*
Project:  VL53L0X Laser Sensor Extendet with Filters
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 06.01.2025
Update:   
Version:  V1.0
IDE:      Arduino IDE 2.3.4

Required Board (Tools -> Board -> Boards Manager...)
- Microcontroller: Wemos d1 mini
- Board: ESP8266 by ESP8266 Community V3.1.2

Required libraries (Tools -> manage libraries)
 - VL53L0X by Pololu V1.3.1
*/

#include <VL53L0X.h>

// Sensor-Instanz erstellen
VL53L0X sensor;

// Einstellungen für den Filter
const uint16_t invalidDistance = 8190;   // Ungültiger Wert für den Sensor
const uint16_t maxValidDistance = 2000;  // Maximal gültige Entfernung in mm
const int numReadings = 10;              // Anzahl der Messungen pro Berechnung
int readings[numReadings];               // Array für Messungen
int readIndex = 0;                       // Index für das Array
int total = 0;                           // Summe der Messungen
int average = 0;                         // Mittelwert der Messungen

// Timing
unsigned long lastMeasurementTime = 0;
const unsigned long measurementInterval = 5000; // 5 Sekunden zwischen Berechnungen

// Zähler für ungültige Messungen
int invalidMeasurementCount = 0;
const int maxInvalidMeasurements = 25;  // Maximale Anzahl ungültiger Messungen

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Sensor initialisieren
  if (!sensor.init()) {
    Serial.println(F("VL53L0X nicht gefunden!"));
    while (1);
  }

  // Long-Range-Modus aktivieren
  sensor.setSignalRateLimit(0.1);              // Standard: 0.25 (reduzieren für höhere Empfindlichkeit)
  sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18); // Standard: 14
  sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14); // Standard: 10
  sensor.setMeasurementTimingBudget(33000);    // Timing-Budget auf 33 ms erhöhen (für größere Reichweite)
}

void loop() {
  unsigned long currentMillis = millis();

  // Alle 5 Sekunden 10 Messungen durchführen
  if (currentMillis - lastMeasurementTime >= measurementInterval) {
    lastMeasurementTime = currentMillis;
    
    // Reset der Summe und der Array-Werte
    total = 0;
    int validReadings = 0;  // Zähler für gültige Messungen
    int i = 0;

    // 10 gültige Messungen durchführen
    while (validReadings < numReadings) {
      uint16_t distance = sensor.readRangeSingleMillimeters();
      
      // Überprüfen auf ungültige Messungen (8190 mm oder mehr als maxValidDistance)
      if (distance == invalidDistance || distance > maxValidDistance) {
        invalidMeasurementCount++;  // Ungültige Messung gezählt
        if (invalidMeasurementCount >= maxInvalidMeasurements) {
          Serial.println(F("Fehler: 25 ungültige Messungen!"));
          return;  // Keine weiteren Messungen durchführen
        }
        continue;  // Ungültige Messung überspringen
      }

      // Messung im Array speichern
      readings[readIndex] = distance;
      readIndex = (readIndex + 1) % numReadings; // Nächster Index im Array

      // Summe der gültigen Messungen berechnen
      total += distance;
      validReadings++;  // Gültige Messung zählen

      delay(300); // Kurze Verzögerung zwischen den Messungen
    }

    // Durchschnitt berechnen
    average = total / numReadings;
    Serial.print(F("Durchschnittliche Entfernung: "));
    Serial.print(average);
    Serial.println(F(" mm"));
  }
}
