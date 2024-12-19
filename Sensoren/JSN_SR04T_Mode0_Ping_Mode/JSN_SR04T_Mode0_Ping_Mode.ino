/*
Project:  JSN-SR04T Mode0: Ping Mode with Wemos d1 mini light
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 19.12.2024
Version:  V1.0 
IDE:      Arduino IDE 2.3.4

Required Board (Tools -> Board -> Boards Manager...)
ESP:      Wemos d1 mini light
  - Board: esp8266 by ESP8266 Community V3.1.2

Required libraries (sketch -> include library -> manage libraries)
 - 
*/

// Pin definition
const uint8_t echoPin = 12; //D6
const uint8_t trigPin = 13; //D7

// Variables
float duration, distance;

void setup() {
  Serial.begin(115200);
  
  // Define pin modes
  pinMode(echoPin, INPUT_PULLUP); // Enable internal pull-up resistor
  pinMode(trigPin, OUTPUT);
}

void loop() {
  digitalWrite(trigPin, LOW);  
  delayMicroseconds(5);

  // Set the trigger pin HIGH for 20us
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(20);
  digitalWrite(trigPin, LOW);
  
  // Measure the time of the incoming pulse
  duration = pulseIn(echoPin, HIGH);

  //Calculate the distance
  distance = duration / 58;
  //distance = duration / 148;  // Distance in inch

  // Print the result to the serial monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  delay(1000); // Wait for 1 second
}
