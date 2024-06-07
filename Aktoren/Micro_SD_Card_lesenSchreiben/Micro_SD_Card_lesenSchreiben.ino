/*
Project:  Micro-SD Card Module - read / write
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 02.06.2024
Version:  V1.0
IDE:      Arduino IDE 2.3.2
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by Espressif Systems   V3.1.2 
 - Board: esp32 by Espressif Systems   V3.0.0 

Required libraries (sketch -> include library -> manage libraries)
 - 
*/

#include <SPI.h>
#include <SD.h>

const uint8_t chipSelect = 2;

void setup() {
  Serial.begin(115200);
  delay(500);
  if (!SD.begin(chipSelect)) {
    Serial.println("Karte konnte nicht gefunden werden!");
    return;
  }

  SD.remove("textfile.txt"); // löscht die Datei wenn sie vorhanden ist

  //Das File öffnen zum beschreiben
  File dataFile = SD.open("textfile.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.println("Dieser Text wurde von Edi für wwww.edistechlab.com geschrieben");
    dataFile.println("für das Kapitel - SD Card Module");
    dataFile.close();
  } else {
    Serial.println("Die Datei textfile.txt konnte nicht geöffnet werden!");
  }

  //Das File öffnen zum lesen
  dataFile = SD.open("textfile.txt", FILE_READ);
  if (dataFile) {
    while (dataFile.available()) {
      char ch = dataFile.read();
      Serial.print(ch);
    }
    dataFile.close();
  } else {
    Serial.println("Die Datei textfile.txt konnte nicht geöffnet werden!");
  }
}

void loop() {
}
