/*
Project:  Batteriespannng messen mit einem ESP8266
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 22.10.2023
Version:  V1.0
IDE:      Arduino IDE 2.1.1
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: Arduino ESP8266 Boards - by ESP8266 Community   V3.1.2

Required libraries (sketch -> include library -> manage libraries)
 - 
Hardware:
Voltage divider R1=15 kOhm / R2=47 kOhm
*/

const int batterieLevelPin = 0; //A0
const int intervallTime = 2000; // millis
unsigned long lastMessureTimeChange = 0;
const float batterMaxVoltage = 4.2;
const float batterMinVoltage = 3.27; 

void setup() {
  Serial.begin(115200);
}
 
void loop() {
  if (millis() - lastMessureTimeChange > intervallTime) {
    int batterieState = analogRead(batterieLevelPin);
    lastMessureTimeChange = millis();
    int batteriePercent = map(batterieState, 0, 1023, 0, 100);
    float batterieVoltage = map(batterieState, 0, 1023, batterMinVoltage, batterMaxVoltage);
    Serial.print("  Batterie Level ist: ");
    Serial.print(batteriePercent);
    Serial.print(" %\f");
    Serial.print("  Batterie Spannung ist: ");
    Serial.print(batterieVoltage);
    Serial.println(" Volt");
  }
}


