/*
Project:  E-ink Display 2.13" BW - Partial update
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

int count = 0;

GxEPD2_BW<GxEPD2_213_BN, GxEPD2_213_BN::HEIGHT> display(GxEPD2_213_BN(/*CS=5*/ 10, /*DC=*/ 9, /*RST=*/ 8, /*BUSY=*/ 7)); // DEPG0213BN 122x250, SSD1680

void setup() {
  display.init();
  display.setRotation(1);
  display.setFont(&FreeSerifBold12pt7b);
  display.setTextColor(GxEPD_BLACK);
  updateScreen();
}

void updateScreen() {  
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(70, 70);
    display.print("Temp:");
    //Temp-Icon
    int x=20 ; int y=30;
    display.fillRoundRect(x, y, 20, 50, 20, 0);
    display.fillCircle(x+10, y+50, 15, 0);
    display.fillRoundRect(x+2, y+2, 16, 46, 20, 1);
    display.fillCircle(x+10, y+50, 13, 1);
    display.fillCircle(x+10, y+50, 8, 0);
    display.fillCircle(x+10, y+50, 5, 1);
    display.fillRect(x+8, y+10, 4, 35, 0);
    display.drawLine(x+15, y+10, x+25, y+10, 0);
    display.drawLine(x+15, y+20, x+25, y+20, 0);
    display.drawLine(x+15, y+30, x+25, y+30, 0);
  }
  while (display.nextPage());
}

void loop() {
  display.setPartialWindow(140, 50, 200, 70);
  display.firstPage();
  count++;
   do{
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(150, 70);
    display.print(count);
   } while(display.nextPage());
  delay(1000);
}
