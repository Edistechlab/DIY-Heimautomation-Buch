/*
Project:  Water level Sensor
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 01.05.2021
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

const int waterAnalogPin = A0;
const int waterSensorPowerPin = 4;
// Kalibrationswerte übertragen - hier mit dem ESP8266
const int waterLevelLow = 50;
const int waterLevelMiddle = 280;
const int waterLevelHigh = 330;

void setup() {
  Serial.begin(115200);  
  pinMode(waterAnalogPin, INPUT);
  pinMode(waterSensorPowerPin, OUTPUT);
}

void loop() {
  digitalWrite(waterSensorPowerPin, HIGH); // Schaltet den Strom für den Sensor ein
  delay(100);
  int waterAnalog = analogRead(waterAnalogPin);
  
  Serial.print("Analog Value: ");
  Serial.print(waterAnalog);

  if (waterAnalog <= waterLevelLow) {
    Serial.println(" - Kein Wasser");
  } else if (waterAnalog > waterLevelLow && waterAnalog <= waterLevelMiddle) {
      Serial.println(" - Der Wasserstand ist tief");
  } else if (waterAnalog > waterLevelMiddle && waterAnalog <= waterLevelHigh) {
      Serial.println(" - Der Wasserstand ist mittel");
  } else if (waterAnalog > waterLevelHigh) {
      Serial.println(" - Der Wasserstand ist hoch");
  }

  digitalWrite(waterSensorPowerPin, LOW);  // Schaltet den Strom für den Sensor aus
  delay(2000);
}
