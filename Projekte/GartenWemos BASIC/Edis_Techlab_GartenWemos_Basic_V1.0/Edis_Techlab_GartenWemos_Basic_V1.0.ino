/*
Project:  Edis Techlab Garten-Wemos Basic
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 22.05.2021
Version:  V1.0
IDE:      Arduino IDE 1.8.13
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V2.7.4 
*/
 
const int sensorTaktSek = 60;         // alle x Sekunden wird der Sensor ausgelesen
const int soilMoistLevelLow = 570;    // Wert ohne Kontakt zum Boden (trocken)
const int soilMoistLevelHigh = 192;   // Wert im Wasser
const int startWatering = 20;         // Schwellwert Bewässerung ein in %
const int wateringDuration = 7;       // Bewässerungszeit in Sekunden
const int maxWateringDay = 2;         // Wie oft pro Tag bewässert werden soll (empfohlen max. 2x)
const int soilMoistPin = A0;          // ADC Pin
const int pumpPin = 5;                // D1

int lastSoilMoist = 0;
int waterPerDayCount = 0;
int sensorTakt = sensorTaktSek * 1000;   // Umrechnung von ms auf Sekunden
long lastMsg = 0;
long todayTimer = 24 * 3600 * 1000 / maxWateringDay;  // Umrechnung von einem Tag auf ms / 24 h * 60 Min * 60 Sek * 1000 durch Anz. Bewässerung
long lastDay = 0;
//char msg[50];

void setup() {
  Serial.begin(115200);
  pinMode(soilMoistPin, INPUT);
  pinMode(pumpPin, OUTPUT);
  digitalWrite(pumpPin, LOW); // Sicherstellen das die Wasserpumpe aus ist
}

void loop() {
  long now = millis();

  if (now - lastMsg > sensorTakt) {   
    lastMsg = now;
    getSoilMoistValues();
  }
  
  if (now - lastDay > todayTimer) {   
    lastDay = now;
    Serial.println("Tagesbewässerungsanzahl zurück gesetzt!");
    waterPerDayCount = 0;
  }
}

void getSoilMoistValues() {
  int newSoilMoist = analogRead(soilMoistPin);

  if (newSoilMoist != lastSoilMoist) {
    Serial.print("Analog Value: ");
    Serial.print(newSoilMoist);
    
    // Auswertung der Bodenfeuchtigkeit in Prozent
    int percentSoilMoist = map(newSoilMoist, soilMoistLevelHigh, soilMoistLevelLow, 100, 0);
    Serial.print("\t"); 
    Serial.print(percentSoilMoist);
    Serial.println(" %");
    if (percentSoilMoist <= startWatering) {
      if (waterPerDayCount < maxWateringDay) {
        Serial.println("Boden zu trocken, Bewässerung aktivieren");
        plantWatering();
      }
    }
    lastSoilMoist = newSoilMoist;
  }
}

void plantWatering() {
  digitalWrite(pumpPin, HIGH); // Schaltet die Wasserpumpe ein
  Serial.println("Bewässerung START");
  delay(wateringDuration*1000);
  
  digitalWrite(pumpPin, LOW); // Schaltet die Wasserpumpe aus
  Serial.println("Bewässerung STOPP");
  ++waterPerDayCount;
  Serial.print("Bewässerung heute: ");
  Serial.println(waterPerDayCount);
  delay(100);
}
