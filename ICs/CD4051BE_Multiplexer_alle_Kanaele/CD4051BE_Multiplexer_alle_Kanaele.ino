/*
Project:  Multiplexer CD4051BE alle 8 Kanäle auslesen - Wemos D1 mini
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 08.07.2021
Version:  V1.0
IDE:      Arduino IDE 1.8.13
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V2.7.4 
*/

//Multiplexer pins
const int A = 0;
const int B = 16;
const int C = 13;
const int Z = A0;

void setup(){
  Serial.begin(115200);
  pinMode(A, OUTPUT); 
  pinMode(B, OUTPUT); 
  pinMode(C, OUTPUT); 
  digitalWrite(A, LOW);
  digitalWrite(B, LOW);
  digitalWrite(C, LOW);
}

void loop(){
  //alle 8 Werte auslesen
  for(int i = 0; i < 8; i++) {
    Serial.print("Kanal ");
    Serial.print(i);
    Serial.print(" ist: ");
    Serial.println(readMux(i));
    delay(1000);
  }
}

int readMux(int Kanal){
  int selectPin[] = {A, B, C};
  int muxKanal[8][3]={
    {0,0,0},    //Kanal 0
    {1,0,0},    //Kanal 1
    {0,1,0},    //Kanal 2
    {1,1,0},    //Kanal 3
    {0,0,1},    //Kanal 4
    {1,0,1},    //Kanal 5
    {0,1,1},    //Kanal 6
    {1,1,1},    //Kanal 7
  };

  //Setzt den ausgewählten Kanal
  for(int i = 0; i < 3; i++) {
    digitalWrite(selectPin[i], muxKanal[Kanal][i]);
  }
  int value = analogRead(Z);
  return value;
}
