/*
Project:  Capacitive soil moist Sensor
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 13.05.2021
Version:  V1.0
IDE:      Arduino IDE 1.8.13
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V2.7.4 
 - Board: esp32   by Espressif Systems   V1.0.4

Wirering for the Module:
Modul       ESP32       ESP8266     UNO         Mega
+           3.3V        3.3V        5V          5V      
-           GND         GND         GND         GND
A           VP/A0/36    A0          A0          A0
*/

const int soilMoistPin = A0;
const int soilMoistLevelLow = 832;    //Dieser Wert soll von euch entsprechend angepasst werden
const int soilMoistLevelHigh = 382;   //Dieser Wert soll von euch entsprechend angepasst werden

void setup() {
  Serial.begin(115200);  
  pinMode(soilMoistPin, INPUT);
}

void loop() {
  int soilMoist = analogRead(soilMoistPin);
  Serial.print("Analog Value: ");
  Serial.print(soilMoist);
  // Auswertung der Bodenfeuchtigkeit in Prozent
  int percentSoilMoist = map(soilMoist, soilMoistLevelHigh, soilMoistLevelLow, 100, 0);
  Serial.print("\t"); 
  Serial.print(percentSoilMoist);
  Serial.println(" %");
  delay(2000);
}
