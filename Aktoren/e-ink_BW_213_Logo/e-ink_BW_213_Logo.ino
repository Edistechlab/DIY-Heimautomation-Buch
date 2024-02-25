/*
Project:  NTC Zeit Server for ESP32 and ESP8266
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 04.06.2023 / update 24.01.2024
Version:  V1.0
IDE:      Arduino IDE 2.3.1
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V3.1.2 
 - Board: esp32   by Espressif Systems   V2.0.11

Required libraries (sketch -> include library -> manage libraries)
 - GxEPD2 V1.5.5 by Jean-Marc Zingg
 - Adafruit GFX Library V1.11.9 by Adafruit
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
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(20, 50);
    display.print("Edi");
    display.drawRoundRect(10, 10, 20, 10, 3, 0);
    display.fillRoundRect(40, 10, 20, 10, 3, 0);

    //Techlab Logo
    int logoX=200; int logoY=55;
    display.drawCircle(logoX, logoY, 5, 0);
    display.drawCircle(logoX, logoY+15, 5, 0);
    display.drawCircle(logoX, logoY+30, 5, 0);
    display.drawLine(logoX-20, logoY, logoX-5, logoY, 0);
    display.drawLine(logoX-20, logoY+15, logoX-5, logoY+15, 0);
    display.drawLine(logoX-20, logoY+30, logoX-5, logoY+30, 0);
    display.drawLine(logoX-20, logoY, logoX-20, logoY+40, 0);
    display.drawLine(logoX-20, logoY+40, logoX-10, logoY+40, 0);
    display.drawLine(logoX-10, logoY+40, logoX-10, logoY+45, 0);
    display.drawLine(logoX-10, logoY+45, logoX-35, logoY+45, 0);
    display.drawLine(logoX-35, logoY+45, logoX-35, logoY+10, 0);
    display.drawLine(logoX-35, logoY+10, logoX-45, logoY+20, 0);
    display.drawLine(logoX-45, logoY+20, logoX-55, logoY+10, 0);
    display.drawLine(logoX-55, logoY+10, logoX-5, logoY-35, 0);
    display.drawLine(logoX-5, logoY-35, logoX+10, logoY-20, 0);
    display.drawLine(logoX+10, logoY-20, logoX+10, logoY-35, 0);
    display.drawLine(logoX+10, logoY-35, logoX+20, logoY-35, 0);
    display.drawLine(logoX+20, logoY-35, logoX+20, logoY-10, 0);
    display.drawLine(logoX+20, logoY-10, logoX+40, logoY+10, 0);
    display.drawLine(logoX+40, logoY+10, logoX+30, logoY+20, 0);
    display.drawLine(logoX+30, logoY+20, logoX+20, logoY+10, 0);
    display.drawLine(logoX+20, logoY+10, logoX+20, logoY+45, 0);
    display.drawLine(logoX+20, logoY+45, logoX, logoY+45, 0);
  }
  while (display.nextPage());
}

void loop() {

}
