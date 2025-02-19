/*
Projekt:  Edis Techlab Garten-Wemos Basic (Optimized)
Autor:    Thomas Edlinger (Optimized by AI o3 mini)
Datum:    Updated 13.02.2025
Version:  V2.1
IDE:      Arduino IDE 2.3.4
*/

#define SENSOR_TAKT_MS         60000      // Sensorabfrage alle 60 Sekunden
#define SOIL_MOIST_LOW         675        // Trockener Boden
#define SOIL_MOIST_HIGH        283        // Nasser Boden
#define START_WATERING         20         // Schwellwert für Bewässerung (%)
#define WATERING_DURATION      70000      // Bewässerungsdauer in Millisekunden (70 Sekunden)
#define TIME_BETWEEN_WATERING  43200000   // 12 Stunden in Millisekunden
#define DAY_DURATION           86400000   // 24 Stunden in Millisekunden
#define MAX_WATERING_DAY       2          // 1 = Nur morgens, 2 = Morgens + Abends
#define NUM_SAMPLES            5          // Anzahl der Messungen für Mittelwertbildung

const int soilMoistPin = A0;            // ADC-Pin für Bodenfeuchtigkeit
const int pumpPin = 5;                  // Pin für Pumpensteuerung

unsigned long lastSensorRead = 0;
unsigned long morningStartTime = 0;     // Wird einmal beim Booten gesetzt
unsigned long pumpStartTime = 0;        // Zeitpunkt, wann die Pumpe gestartet wurde
bool morningWatered = false;
bool eveningWatered = false;
bool isWatering = false;                // Status der Pumpe

void setup() {
  Serial.begin(115200);
  pinMode(soilMoistPin, INPUT);
  pinMode(pumpPin, OUTPUT);
  digitalWrite(pumpPin, LOW);  // Pumpe ausschalten

  // Startzeit einmalig beim Booten setzen
  morningStartTime = millis();
  morningWatered = false;
  eveningWatered = false;
  Serial.println("Startzeit gesetzt! Bewässerung startet nach Zeitplan.");
}

void loop() {
  unsigned long now = millis();

  // Täglicher Reset: Nach 24 Stunden werden die Bewässerungsflags zurückgesetzt
  if (now - morningStartTime >= DAY_DURATION) {
    morningStartTime = now;
    morningWatered = false;
    eveningWatered = false;
    Serial.println("Neuer Tag: Bewässerungsstatus zurückgesetzt.");
  }

  // Nicht-blockierende Steuerung: Schalte Pumpe aus, wenn die Bewässerungsdauer abgelaufen ist
  if (isWatering && (now - pumpStartTime >= WATERING_DURATION)) {
    stopWatering();
  }

  // Bestimme, ob aktuell ein Bewässerungsfenster aktiv ist
  bool wateringWindow = false;
  if (MAX_WATERING_DAY == 1) {
    wateringWindow = !morningWatered;
  } else if (MAX_WATERING_DAY == 2) {
    if (!morningWatered) {
      wateringWindow = true;
    } else if (!eveningWatered && (now - morningStartTime >= TIME_BETWEEN_WATERING)) {
      wateringWindow = true;
      Serial.println("12 Stunden vergangen, Abendbewässerung prüfen...");
    }
  }

  if (!wateringWindow) {
    return;
  }

  // Regelmäßige Sensorabfrage
  if (now - lastSensorRead >= SENSOR_TAKT_MS) {
    lastSensorRead = now;
    checkSoilMoisture();
  }
}

void checkSoilMoisture() {
  int rawSoilMoist = readSoilMoisture();
  int percentSoilMoist = map(rawSoilMoist, SOIL_MOIST_HIGH, SOIL_MOIST_LOW, 100, 0);

  Serial.print("Analogwert: ");
  Serial.print(rawSoilMoist);
  Serial.print("\tBodenfeuchte: ");
  Serial.print(percentSoilMoist);
  Serial.println(" %");

  if (percentSoilMoist <= START_WATERING) {
    if (MAX_WATERING_DAY == 1) {
      Serial.println("Morgenbewässerung starten...");
      startWatering();
      morningWatered = true;
    } else if (MAX_WATERING_DAY == 2) {
      if (!morningWatered) {
        Serial.println("Morgenbewässerung starten...");
        startWatering();
        morningWatered = true;
      } else if (!eveningWatered) {
        Serial.println("Abendbewässerung starten...");
        startWatering();
        eveningWatered = true;
      }
    }
  } else {
    Serial.println("Bodenfeuchte ausreichend. Keine Bewässerung erforderlich.");
  }
}

int readSoilMoisture() {
  long sum = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    sum += analogRead(soilMoistPin);
    delay(10); // Kleine Pause zur Rauschminderung
  }
  return sum / NUM_SAMPLES;
}

void startWatering() {
  digitalWrite(pumpPin, HIGH);
  Serial.println("Bewässerung START");
  pumpStartTime = millis();
  isWatering = true;
}

void stopWatering() {
  digitalWrite(pumpPin, LOW);
  Serial.println("Bewässerung STOPP");
  isWatering = false;
}
