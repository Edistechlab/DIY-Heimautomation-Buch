/*
Project:  Raindrop Sensor - MH-RD / YL-83 / FC-37
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 25.04.2021
Version:  V1.0
IDE:      Arduino IDE 1.8.13
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V2.7.4 
 - Board: esp32   by Espressif Systems   V1.0.4

Wirering for the Module:
Modul       ESP32       ESP8266     UNO         Mega
VCC         D4/4        D2/4        4           4      
GND         GND         GND         GND         GND
DO          D5/5        D1/5        5           5
AO          VP/A0/36    A0          A0          A0
*/

const int rainDigitalPin = 5;
const int rainAnalogPin = A0;
const int rainSensorPowerPin = 4;

void setup() {
  Serial.begin(115200);  
  pinMode(rainDigitalPin, INPUT);
  pinMode(rainAnalogPin, INPUT);
  pinMode(rainSensorPowerPin, OUTPUT);
}

void loop() {
  digitalWrite(rainSensorPowerPin, HIGH); // Schaltet den Strom für den Sensor ein
  delay(100);
  int rainDigital = digitalRead(rainDigitalPin);
  int rainAnalog = analogRead(rainAnalogPin);
  
  Serial.print("Analog Value: ");
  Serial.print(rainAnalog);
  Serial.print("\t Digital Value: "); 
  Serial.println(rainDigital);            // 0 = Regen / 1 = kein Regen

  digitalWrite(rainSensorPowerPin, LOW);  // Schaltet den Strom für den Sensor aus
  delay(2000);
}
