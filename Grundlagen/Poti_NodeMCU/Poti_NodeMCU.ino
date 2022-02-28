/*
Project:  Poti am NodeMCU ESP8266  
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 28.02.2022
Version:  V1.0
*/

int potpin = 0;   //Analog pin A0 für das Potenziometer

void setup() {
  Serial.begin(115200);
}

void loop() {
  Serial.println(analogRead(potpin)); // (Wert zwischen 0 und 1023)
  delay(500);
}
