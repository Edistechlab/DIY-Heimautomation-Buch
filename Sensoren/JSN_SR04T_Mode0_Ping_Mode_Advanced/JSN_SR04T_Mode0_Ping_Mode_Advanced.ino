/*
Project:  JSN-SR04T Mode0: Ping Mode Advanced - calculate the speed of sound based on temperature and humidity
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

// Constans
const unsigned long timeout = 30000;

// Variables
float duration = 0;       // Time for the echo signal to return
float distance = 0;       // Calculated distance
float temperature = 25.0; // Ambient temperature in °C
float humidity = 50.0;    // Relative humidity in %
float speedOfSound = 0;   // Speed of sound in m/s

void setup() {
  Serial.begin(115200);
  
  // Define pin modes
  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);
}

void loop() {
  // Calculate the speed of sound
  speedOfSound = calculateSpeedOfSound(temperature, humidity);

  // Generate the Trigger-Signal
  digitalWrite(trigPin, LOW);  
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(20);
  digitalWrite(trigPin, LOW);
  
  // Measure the time of the incoming pulse
  duration = pulseIn(echoPin, HIGH, timeout);

  // Check if an echo signal was received
  if (duration == 0) {
  Serial.println("No echo received!");
  return;
  }

  //Calculate the distance
  distance = (duration / 2.0) * (speedOfSound / 10000.0); // Convert to cm

  // Print the result to the serial monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  delay(1000); // Wait for 1 second
}

// Function to calculate the speed of sound based on temperature and humidity
float calculateSpeedOfSound(float temp, float hum) {
  // Calculate the saturation vapor pressure (e) in hPa
  float e = 6.11 * pow(10, (7.5 * temp) / (temp + 237.3));

  // Atmospheric pressure (standard value in hPa)
  float P = 1013.25;

  // Humidity correction factor
  float KH = (0.0124 * hum * e) / P;

  // Calculate the speed of sound
  return 331.3 * sqrt(1 + temp / 273.15) + (0.6 * temp) + KH;
}
