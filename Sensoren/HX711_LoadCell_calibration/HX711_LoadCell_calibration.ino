 /*
Project:  HX711 - Load-cell calibration
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 27.01.2026
Update:   
Version:  V1.0
IDE:      Arduino IDE 2.3.5

Required Board (Tools -> Board -> Boards Manager...)
- Microcontroller: Arduino Nano ESP32
- Board: Arduino ESP32 Boards by Arduino V2.0.18-arduino.5

Required libraries (Tools -> manage libraries)
 - HX711 Arduino Library by Bogdan Necula V0.7.5
*/

#include "HX711.h"

// ================= PIN DEFINITIONS =================
#define HX711_DT   2   // DOUT / DT
#define HX711_SCK  3   // CLK / SCK

#define REF_WEIGHT_GRAMS 1000.0   // Referenzgewicht (1 kg)

HX711 scale;

long raw_zero = 0;
long raw_with_weight = 0;
bool step_done = false;

void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.println("\n=== HX711 KALIBRIERUNG ===");

  scale.begin(HX711_DT, HX711_SCK);

  while (!scale.is_ready()) {
    delay(100);
  }

  // ---------- STEP 1: TARA ----------
  Serial.println("1) Waage LEER lassen");
  delay(3000);

  scale.tare();
  raw_zero = scale.read_average(20);

  Serial.print("RAW (leer): ");
  Serial.println(raw_zero);

  Serial.println("\n2) Jetzt GENAU 1 kg auflegen");
  Serial.println("   Warte auf Gewicht...");
}

void loop() {
  if (step_done) return;

  // warten bis Gewicht stabil ist
  if (scale.is_ready()) {
    long raw = scale.read_average(20);

    // simple Schwellwert-Erkennung
    if (abs(raw - raw_zero) > 10000) {
      raw_with_weight = raw;
      step_done = true;

      Serial.print("\nRAW (mit 1 kg): ");
      Serial.println(raw_with_weight);

      long diff = raw_with_weight - raw_zero;

      Serial.print("RAW Differenz: ");
      Serial.println(diff);

      float calibration_factor = diff / REF_WEIGHT_GRAMS;

      Serial.println("\n=== ERGEBNIS ===");
      Serial.print("Kalibrierfaktor = ");
      Serial.println(calibration_factor, 3);

      Serial.println("\n➡ Diesen Wert in deinem Code eintragen:");
      Serial.print("float calibration_factor = ");
      Serial.print(calibration_factor, 3);
      Serial.println(";");

      Serial.println("\nKalibrierung abgeschlossen ✔");
    }
  }

  delay(500);
}
