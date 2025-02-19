/*
Project:  Edis Techlab Garten-Wemos Basic (Optimized)
Author:   Thomas Edlinger (Optimized by AI)
Date:     Updated 13.02.2025
Version:  V2.0
IDE:      Arduino IDE 2.3.4
*/

#define SENSOR_TAKT_MS       60000      // Sensorabfrage alle 60 Sekunden
#define SOIL_MOIST_LOW       675        // Trockener Boden
#define SOIL_MOIST_HIGH      283        // Nasser Boden
#define START_WATERING       20         // Schwellwert für Bewässerung (%)
#define WATERING_DURATION    70000      // Bewässerungsdauer in Millisekunden (70 Sekunden)
#define TIME_BETWEEN_WATERING 43200000   // 12 Stunden (12*60*60*1000 ms)
#define MAX_WATERING_DAY     2          // 1 = Nur morgens, 2 = Morgens + Abends

const int soilMoistPin = A0;            // ADC Pin
const int pumpPin = 5;                  // D1 (Pumpensteuerung)

unsigned long lastSensorRead = 0;
unsigned long morningStartTime = 0;  // Vom Benutzer durch Reset gesetzt
unsigned long pumpStartTime = 0;     // Zeitpunkt, wann die Pumpe gestartet wurde
bool morningWatered = false;
bool eveningWatered = false;
bool isWatering = false;             // Statusvariable für die Pumpe

void setup() {
  Serial.begin(115200);
  pinMode(soilMoistPin, INPUT);
  pinMode(pumpPin, OUTPUT);
  digitalWrite(pumpPin, LOW);  // Sicherstellen, dass die Pumpe aus ist

  // Benutzer muss den Wemos am Morgen EINMAL resetten, um die Startzeit zu setzen
  morningStartTime = millis();
  morningWatered = false;
  eveningWatered = false;
  Serial.println("Startzeit gesetzt! Bewässerung startet jetzt nach Zeitplan.");
}

void loop() {
  unsigned long now = millis();

  // Feuchtigkeitsmessung in regelmäßigen Abständen
  if (now - lastSensorRead >= SENSOR_TAKT_MS) {
    lastSensorRead = now;
    checkSoilMoisture();
  }

  // Falls nur eine Bewässerung pro Tag erlaubt ist, wird nur morgens gewässert
  if (MAX_WATERING_DAY == 1) {
    if (!morningWatered) {
      checkSoilMoisture();
    }
  } 
  // Falls zwei Bewässerungen erlaubt sind, wird morgens und abends gewässert
  else if (MAX_WATERING_DAY == 2) {
    if (!morningWatered) {
      checkSoilMoisture();
    } 
    else if (!eveningWatered && now - morningStartTime >= TIME_BETWEEN_WATERING) {
      Serial.println("12 Stunden vergangen, Abendbewässerung prüfen...");
      checkSoilMoisture();
    }
  }

  // Nicht-blockierendes Abschalten der Pumpe nach Bewässerungszeit
  if (isWatering && now - pumpStartTime >= WATERING_DURATION) {
    stopWatering();
  }
}

void checkSoilMoisture() {
  int rawSoilMoist = readSoilMoisture();
  int percentSoilMoist = map(rawSoilMoist, SOIL_MOIST_HIGH, SOIL_MOIST_LOW, 100, 0);

  Serial.print("Analog Value: ");
  Serial.print(rawSoilMoist);
  Serial.print("\t");
  Serial.print(percentSoilMoist);
  Serial.println(" %");

  if (percentSoilMoist <= START_WATERING) {
    if (!morningWatered) {
      Serial.println("Morgenbewässerung starten...");
      startWatering();
      morningWatered = true;
    } 
    else if (!eveningWatered && MAX_WATERING_DAY == 2) {
      Serial.println("Abendbewässerung starten...");
      startWatering();
      eveningWatered = true;
    }
  }
}

int readSoilMoisture() {
  // Durchschnitt von 5 Messungen zur Glättung von Rauschen
  int sum = 0;
  for (int i = 5; i > 0; i--) {
    sum += analogRead(soilMoistPin);
    delay(10); // Kleine Pause zwischen Messungen
  }
  return sum / 5;
}

void startWatering() {
  digitalWrite(pumpPin, HIGH);
  Serial.println("Bewässerung START");
  pumpStartTime = millis();  // Startzeitpunkt speichern
  isWatering = true;         // Status setzen
}

void stopWatering() {
  digitalWrite(pumpPin, LOW);
  Serial.println("Bewässerung STOPP");
  isWatering = false;
}
