/*
Project:  Calibrarion of the soil moist Sensor
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 23.05.2021
Version:  V1.0
IDE:      Arduino IDE 1.8.13
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V2.7.4 
*/

const int soilMoistPin = A0;
int soilMoistLow[10];
int summeAnalogLow = 0;
int soilMoistHigh[10];
int summeAnalogHigh = 0;

void setup() {
  Serial.begin(115200);  
  pinMode(soilMoistPin, INPUT);
}

void loop() {
  delay(1000);
  Serial.println(" ");
  Serial.println("Sensor ohne Boden/Wasser Kontakt, die Messung startet");
  counter();
  
  for (int i = 0; i <= 9; i++) {
    soilMoistLow[i] = analogRead(soilMoistPin);
    Serial.print("Messwert ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(soilMoistLow[i]);
    summeAnalogLow = summeAnalogLow + soilMoistLow[i];
    delay(2000);
  }
  
  Serial.println(" ");
  Serial.println("Sensor ins Wasser tauchen, die Messung startet");
  counter();

  for (int i = 0; i <= 9; i++) {
    soilMoistHigh[i] = analogRead(soilMoistPin);
    Serial.print("Messwert ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(soilMoistHigh[i]);
    summeAnalogHigh = summeAnalogHigh + soilMoistHigh[i];
    delay(2000);
  }

  int mittelwertLow = summeAnalogLow / 10;
  int mittelwertHigh = summeAnalogHigh / 10; 
  Serial.println(" ");
  Serial.print("Diesen Wert bei soilMoistLevelLow eintragen: ");
  Serial.println(mittelwertLow);
  Serial.print("Diesen Wert bei soilMoistLevelHigh eintragen: ");
  Serial.println(mittelwertHigh);
  Serial.println(" ");
  delay(20000); 
  Serial.println("erneute Messung ");
  counter();
}

void counter(){
  for (int i = 10; i >= 1; i--) {
    Serial.print("... in: ");
    Serial.println(i);
    delay(1000);
  }
}
