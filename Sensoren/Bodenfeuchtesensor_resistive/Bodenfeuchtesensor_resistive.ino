/*
Project:  resistiv soil moist Sensor
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 08.05.2021
Version:  V1.0
IDE:      Arduino IDE 1.8.13
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V2.7.4 
 - Board: esp32   by Espressif Systems   V1.0.4

Wirering for the Module:
Modul       ESP32       ESP8266     UNO         Mega
VCC         D4/4        D2/4        4           4      
GND         GND         GND         GND         GND
AO          VP/A0/36    A0          A0          A0
*/

const int soilMoistPin = A0;
const int sensorPowerPin = 4;
const int soilMoistLevelLow = 250;    //Dieser Wert soll von euch entsprechend angepasst werden
const int soilMoistLevelHigh = 400;   //Dieser Wert soll von euch entsprechend angepasst werden

void setup() {
  Serial.begin(115200);  
  pinMode(soilMoistPin, INPUT);
  pinMode(sensorPowerPin, OUTPUT);
}

void loop() {
  digitalWrite(sensorPowerPin, HIGH); // Schaltet den Strom für den Sensor ein
  delay(100);
  int soilMoist = analogRead(soilMoistPin);
  
  Serial.print("Analog Value: ");
  Serial.print(soilMoist);
    // Auswertung der Bodenfeuchtigkeit
  if (soilMoist > soilMoistLevelHigh) {
    Serial.println(" Status: Der Boden ist zu feucht");
  } else if (soilMoist >= soilMoistLevelLow && soilMoist < soilMoistLevelHigh) {
    Serial.println(" Status: Die Bodenfeuchtigkeit ist perfekt");
  } else {
    Serial.println(" Status: Der Boden ist zu trocken");
  }

  digitalWrite(sensorPowerPin, LOW);  // Schaltet den Strom für den Sensor aus
  delay(2000);
}
