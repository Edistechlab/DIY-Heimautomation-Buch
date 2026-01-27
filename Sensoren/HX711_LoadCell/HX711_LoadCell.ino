 /*
Project:  HX711 - Load-cell
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

// ================= KALIBRIERUNG =================
// Dieser Wert MUSS später angepasst werden!
float calibration_factor = -7050.0;

// ================= OBJECT =================
HX711 scale;

void setup() {
  Serial.begin(115200);
  Serial.println("HX711 Gewichtsmessung startet...");

  scale.begin(HX711_DT, HX711_SCK);

  delay(500);

  // Nullpunkt setzen (Waage unbelastet!)
  scale.set_scale(calibration_factor);
  scale.tare();

  Serial.println("Waage tariert.");
}

void loop() {
  if (scale.is_ready()) {
    float weight = scale.get_units(5); // Mittelwert aus 5 Messungen

    Serial.print("Gewicht: ");
    Serial.print(weight);
    Serial.println(" g");
  } else {
    Serial.println("HX711 nicht bereit");
  }

  delay(500);
}
