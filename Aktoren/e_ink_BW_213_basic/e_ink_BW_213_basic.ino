/*
Project:  E-ink Display 2.13" BW - Basic screen
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 18.02.2024
Version:  V1.0
IDE:      Arduino IDE 2.3.1
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V3.1.2 
 - Board: esp32   by Espressif Systems   V2.0.11

Required libraries (sketch -> include library -> manage libraries)
 - GxEPD2 by Jean-Marc Zingg V1.5.5 
 - Adafruit GFX Library by Adafruit V1.11.9
*/

#include <GxEPD2_BW.h>
#include <Fonts/FreeSerifBold12pt7b.h>

GxEPD2_BW<GxEPD2_213_BN, GxEPD2_213_BN::HEIGHT> display(GxEPD2_213_BN(/*CS=5*/ 10, /*DC=*/ 9, /*RST=*/ 8, /*BUSY=*/ 7)); // DEPG0213BN 122x250, SSD1680

void setup() {
  display.init();
  display.setRotation(1);
  display.setFont(&FreeSerifBold12pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setFullWindow();
  updateScreen();
  display.hibernate();
}

void updateScreen() {  
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(60, 70);
    display.print("Edi's Techlab");
    display.drawRoundRect(10, 10, 20, 10, 3, 0);
    display.fillRoundRect(40, 10, 20, 10, 3, 0);
    display.drawTriangle(180, 100, 190, 110, 220, 90, 0);
  }
  while (display.nextPage());
}

void loop() {
}
