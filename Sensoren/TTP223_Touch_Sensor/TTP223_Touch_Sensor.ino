 /*
Project:  TTP223 Touch Sensor
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 12.01.2026
Update:   
Version:  V1.0
IDE:      Arduino IDE 2.3.5

Required Board (Tools -> Board -> Boards Manager...)
- Microcontroller: Arduino Nano ESP32
- Board: Arduino ESP32 Boards by Arduino V2.0.18-arduino.5

Required libraries (Tools -> manage libraries)
 - 
*/

const uint8_t TOUCH_PIN = 2;
const uint8_t LED_PIN   = 7;

void setup() {
  pinMode(TOUCH_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  bool touchState = digitalRead(TOUCH_PIN); // For active LOW, invert the input using !digitalRead()
  digitalWrite(LED_PIN, touchState ? HIGH : LOW);
  delay(100);
}