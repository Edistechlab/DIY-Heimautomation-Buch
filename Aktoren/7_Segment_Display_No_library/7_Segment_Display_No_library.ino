/*
Project:  7-Segment Display ohne SevSeg Library
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 26.09.2020
Version:  V1.0
IDE:      Arduino IDE 1.8.13
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V2.7.4 
 - Board: esp32   by Espressif Systems   V1.0.2

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

int numArray[11][8] = {  
  { 1,1,1,1,1,1,0,0 },  // 0
  { 0,1,1,0,0,0,0,0 },  // 1
  { 1,1,0,1,1,0,1,0 },  // 2
  { 1,1,1,1,0,0,1,0 },  // 3
  { 0,1,1,0,0,1,1,0 },  // 4
  { 1,0,1,1,0,1,1,0 },  // 5
  { 1,0,1,1,1,1,1,0 },  // 6
  { 1,1,1,0,0,0,0,0 },  // 7
  { 1,1,1,1,1,1,1,0 },  // 8
  { 1,1,1,0,0,1,1,0 },  // 9
  { 0,0,0,0,0,0,0,1 }   // Punkt
}; 

int pinArray[] = {2, 3, 4, 5, 6, 7, 8, 9};           // Arduinos
//int pinArray[] = {16, 17, 5, 18, 19, 21, 22, 23};  // ESP32
//int pinArray[] = {5, 4, 0, 2, 12, 13, 15, 14};     // ESP8266

void setup() { 
  for (int i = 0; i < 8; i++) {
    pinMode(pinArray[i], OUTPUT);
  }
}
  
void loop() {
  numWrite(8);
  delay(1000);
}
 
void numWrite(int number) {
  for (int i = 0; i < 8; i++) {
    digitalWrite(pinArray[i], numArray[number][i]);
  }
}
