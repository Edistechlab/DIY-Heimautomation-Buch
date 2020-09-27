/*
Project:  7-Segment Display
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 26.09.2020
Version:  V1.0
IDE:      Arduino IDE 1.8.13
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V2.7.4 
 - Board: esp32   by Espressif Systems   V1.0.2

Required libraries (sketch -> include library -> manage libraries)
 - SevSegShift Library by Dean Reading V3.4.3

Wirering for the 7-Segment Display:
7-Segment   ESP32       ESP8266     UNO         Mega
COM         3.3V/GND    3.3V/GND    5V/GND      5V/GND
A           16          5 (D1)      2           2
B           17          4 (D2)      3           3
C           5           0 (D3)      4           4
D           18          2 (D4)      5           5
E           19          12 (D6)     6           6
F           21          13 (D7)     7           7
G           22          15 (D8)     8           8
DP          23          14 (D5)     9           9
*/

#include "SevSeg.h"
SevSeg sevseg;

void setup(){
  byte numDigits = 1;     // Anzahl der 7-Segment Anzeigen
  byte digitPins[] = {};  // Bleibt leer bei nur einem Display

  //Pin Definition in der Reihenfolge: A, B, C, D, E, F, G, DP
  byte segmentPins[] = {16, 17, 5, 18, 19, 21, 22, 23};   // NodeMCU ESP32
  //byte segmentPins[] = {2, 3, 4, 5, 6, 7, 8, 9};        // Arduino Uno und Mega 2560
  //byte segmentPins[] = {5, 4, 0, 2, 12, 13, 15, 14};    // NodeMCU ESP8266
  
  bool resistorsOnSegments = true;

  //Auswahl Common Anode oder Common Cathode 7-Segment Anzeige
  sevseg.begin(COMMON_CATHODE, numDigits, digitPins, segmentPins, resistorsOnSegments);
  //sevseg.begin(COMMON_ANODE, numDigits, digitPins, segmentPins, resistorsOnSegments);
  sevseg.setBrightness(90);
}

void loop(){ 
   // Zahlen von 0-9 
   for(int i = 0; i < 10; i++){
     sevseg.setNumber(i, 0);  //zweite Stelle ist der DP, 0=EIN  1=AUS
     sevseg.refreshDisplay(); 
     delay(1000);
   }
   //Schreiben auf der 7-Segment Anzeige
   //a,b,c,d,e,f,g,h,i,j,l,n,o,p,q,r,s,t,u,v=u,y,z=2   -k,m,v,w,x,z
   sevseg.setChars("b"); sevseg.refreshDisplay(); delay(1000); 
   sevseg.setChars("e"); sevseg.refreshDisplay(); delay(2000);  //doppel E
   sevseg.setChars("r"); sevseg.refreshDisplay(); delay(1000);
}
