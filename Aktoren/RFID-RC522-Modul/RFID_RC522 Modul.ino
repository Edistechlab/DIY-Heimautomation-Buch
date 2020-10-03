/*
Project:  RFID Access with ESP8266 / ESP32
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 21.05.2020 
Version:  V1.0
 
- Used Aduino IDE V1.8.12
Required libraries (Tools -> manage libraries)
- MFRC522 by GithubCommunity V1.4.6 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: ESP8266 by ESP8266 Community V2.6.3
 - Board: ESP32 by Espressif Systems V1.0.2

Wirering with different Modules
-RC522      -ESP8266/MCU      -ESP32        -Arduino UNO    -Arduino Mega
VCC         3.3V              3.3V          3.3V            3.3V
RST         D1 / GPIO 5       GPIO 22       GPIO 9          GPIO 5     <--anpassen
GND         GND               GND           GND             GND
MISO        D6 / GPIO 12      GPIO 19       GPIO 12         GPIO 50
MOSI        D7 / GPIO 13      GPIO 23       GPIO 11         GPIO 51
SCK         D5 / GPIO 14      GPIO 18       GPIO 13         GPIO 52
SDA (SS)    D2 / GPIO 4       GPIO 21       GPIO 10         GPIO 53    <--anpassen
IRQ         -                 -             -               -
*/

//Diese Pins müssen angepasst werden (GPIO Pin von der Liste oben)
#define RST_PIN     5          
#define SS_PIN      4 

#include <SPI.h>      
#include <MFRC522.h>  

MFRC522 mfrc522(SS_PIN, RST_PIN); 

void setup() {
  Serial.begin(115200);   
  SPI.begin();      
  mfrc522.PCD_Init();   
}

void accessGranted() {
  Serial.println(" Access Granted ");
  Serial.println();
  delay(3000);
}

void accessRefused() {
   Serial.println(" Access Refused ");
   delay(3000);
}

void loop() {
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  //Zeigt die UID im serial monitor
  Serial.println();
  Serial.print(" UID tag  :");
  String content= "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  Serial.println();
  Serial.print(" PICC type: ");
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));
  
  if (content.substring(1) == "39 7B 02 64" || content.substring(1) == "B2 38 DB 0E") {
    accessGranted();
  }
  else {
    accessRefused();
  }
} 
