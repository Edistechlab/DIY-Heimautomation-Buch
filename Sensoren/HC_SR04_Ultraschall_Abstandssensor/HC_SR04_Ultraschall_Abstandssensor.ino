/*
Project:  HC-SR04 Ultraschall Abstandssensor
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 17.08.2021
Version:  V1.0
IDE:      Arduino IDE 1.8.15
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V3.0.2 
 - Board: esp32   by Espressif Systems   V1.0.6

Required libraries (sketch -> include library -> manage libraries)
 - no library

Connect the Ultrasonic Sensor:
HC-SR04     ESP32       ESP8266     UNO      Mega
VCC         Vin (5V)    Vin (5V)    5V       5V
Trig        D5/GPIO5    D1/GPIO5    5        5
Echo        D4/GPIO4    D2/GPIO4    4        4
GND         GND         GND         GND      GND             

Use a voltage divider for the Echo Pin on the ESPs. 
1 kOhm and 2 kOhm for example

HC-SR04-Echo---1kOhm---2.2kOHm---GND
                     |
                     |
                   GPIO5
*/

const int trigPin = 5;
const int echoPin = 4;
long dauer;
float abstand;

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  dauer = pulseIn(echoPin, HIGH);
  abstand = dauer * 0.034 / 2;

  Serial.print("Abstand zum Objekt = ");
  Serial.print(abstand);
  Serial.println(" cm");
  delay(1000);
}
