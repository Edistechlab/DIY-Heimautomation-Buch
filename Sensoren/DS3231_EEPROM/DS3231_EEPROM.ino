/*
Project:  DS3231 - EEPROM lesen und schreiben, basiert auf dem uEEPROMLib Beispiel Code
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 05.02.2023
Version:  V1.0
IDE:      Arduino IDE 2.0.3
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V3.1.1 

Required libraries (sketch -> include library -> manage libraries)
- uEEPROMLib by Naguissa V1.2.1

Wirering the MCP23017 Modul:
MCP23017    ESP8266    
VCC         3.3V            
GND         GND        
SCL         D1/5       
SDA         D2/4   
*/

#include "uEEPROMLib.h"
uEEPROMLib eeprom(0x57);

void setup() {
  Serial.begin(115200);
  delay(2500);
  Wire.begin();

  int inttmp = 12345;
  float floattmp = 3.1416;
  char chartmp = 'E';
  char c_string[] = "edistechlab.com";
  int string_length = strlen(c_string);
 
  // Einen int Wert speichern
  if (!eeprom.eeprom_write(0, inttmp)) {
    Serial.println("Fehler beim speichern vom int.");
  } else {
    Serial.println("int Wert wurde gespeichert.");
  }

// Einen float Wert speichern
  if (!eeprom.eeprom_write(4, floattmp)) {
    Serial.println("Fehler beim speichern vom float.");
  } else {
    Serial.println("float Wert wurde gespeichert.");
  }

  // Einen char Wert speichern 
  if (!eeprom.eeprom_write(8, chartmp)) {
  Serial.println("Fehler beim speichern vom char.");
  } else {
  Serial.println("char Wert wurde gespeichert.");
  }

  // Einen string Wert speichern 
  if (!eeprom.eeprom_write(33, (byte *) c_string, strlen(c_string))) {
  Serial.println("Fehler beim speichern vom string.");
  } else {
  Serial.println("string Wert wurde gespeichert.");
  }

  // Werte vom EEPROM auslesen.
  Serial.println("");
  Serial.println("Speicher auslesen...");
  
  Serial.print("int: ");
  eeprom.eeprom_read(0, &inttmp);
  Serial.println(inttmp);

  Serial.print("float: ");
  eeprom.eeprom_read(4, &floattmp);
  Serial.println((float) floattmp);

  Serial.print("char: ");
  eeprom.eeprom_read(8, &chartmp);
  Serial.println(chartmp);

  Serial.print("string: ");
  eeprom.eeprom_read(33, (byte *) c_string, string_length);
  Serial.println(c_string);
}

void loop() {
}
