/*
Project:  Gassensor - MQ-x (MQ-2)
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 27.10.2021
Version:  V1.0
IDE:      Arduino IDE 1.8.16
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V3.0.2 
 - Board: esp32   by Espressif Systems   V1.0.6

Wirering for the Module:
Modul       ESP32       ESP8266     UNO         Mega
VCC         5V          5V          5V          5V       
GND         GND         GND         GND         GND
DO          D4/4        D2/4        4           4
AO          VP/A0/36    A0          A0          A0
*/

const int mq2_DigitalPin = 4;
const int mq2_AnalogPin = A0;
const int sensorThres = 400;

void setup() {
  Serial.begin(115200);  
  pinMode(mq2_DigitalPin, INPUT);
  pinMode(mq2_AnalogPin, INPUT);
  delay (180000); // Beim ersten Start 3 Min. warten.
}

void loop() {
  int mq2_Digital = digitalRead(mq2_DigitalPin);
  int mq2_Analog = analogRead(mq2_AnalogPin);
  
  Serial.print("Analog Value: ");
  Serial.print(mq2_Analog);
  Serial.print("\t Digital Value: "); 
  Serial.println(mq2_Digital);   // 0 = Kein Gas/Rauch / 1 = Gas/Rauch
  if (mq2_Analog > sensorThres) {
    Serial.println("Analogwert ist überschritten!");
  }
  delay(1000);
}
