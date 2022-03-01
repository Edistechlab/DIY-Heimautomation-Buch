/*
Project:  Poti am NodeMCU ESP8266 - JSON senden
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 28.02.2022
Version:  V1.0
*/

#include <ArduinoJson.h>
int potpin = 0;   //Analog pin A0 für das Potenziometer
StaticJsonDocument<50> doc;
String meinJson; 

void setup() {
  Serial.begin(115200);
}

void loop() {
  meinJson = "";
  if (analogRead(potpin)<=500) {
    doc["ledRot"] = true;
    doc["ledGruen"] = false;
  }
  else {
    doc["ledRot"] = false;
    doc["ledGruen"] = true;
  }
  doc["poti"] = analogRead(potpin);
  serializeJson(doc, meinJson);
  Serial.println(meinJson);
  delay(500);
}
