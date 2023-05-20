/*
Project:  Interrupt Routinge - Taster schaltet LED
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 19.05.2023
Version:  V1.0
IDE:      Arduino IDE 2.1.0
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V3.1.2 

Required libraries (sketch -> include library -> manage libraries)
*/

const byte buttonPin = 5; //D1 - Wemos d1 mini
const byte LEDPin = 4; //D2  - Wemos d1 mini
byte lastButtonState = 0;
const int debounceTime = 30; // millis
byte toggleState = 0;
unsigned long lastButtonTimeChange = 0;
volatile int buttonFlag;

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(LEDPin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(buttonPin), ISR_button, CHANGE);
}

IRAM_ATTR void ISR_button() {
  buttonFlag = 1;
}

void loop() {
  if (millis() - lastButtonTimeChange > debounceTime && buttonFlag) {
    lastButtonTimeChange = millis();                                                   
    if(digitalRead(buttonPin) == 0 && lastButtonState == 1){
      toggleState =! toggleState;
      digitalWrite(LEDPin, toggleState);
      lastButtonState = 0;
    }
    else if(digitalRead(buttonPin) == 1 && lastButtonState == 0) {
      lastButtonState = 1;
    }
    buttonFlag = 0;
  }
}