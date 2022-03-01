/*
Project:  Wemos d1 mini - JSON Daten auflösen
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 28.02.2022
Version:  V1.0
*/

#include <ArduinoJson.h>
StaticJsonDocument<100> doc;
String json = "{\"ledRot\":0,\"ledGruen\":1,\"poti\":680}";

void setup() {
  Serial.begin(115200);
}

void loop() {
  DeserializationError error = deserializeJson(doc, json);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
  bool LEDRot = doc["ledRot"];
  bool LEDGruen = doc["ledGruen"];
  int Poti = doc["poti"];
  Serial.println(LEDRot);
  Serial.println(LEDGruen);
  Serial.println(Poti);
  delay(1000);
}
