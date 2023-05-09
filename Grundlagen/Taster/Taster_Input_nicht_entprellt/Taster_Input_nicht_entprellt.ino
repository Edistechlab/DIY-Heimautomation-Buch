const byte buttonPin = 5; //D1
byte lastButtonState = LOW;
int numberKeyPresses = 0;

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP); 
}

void loop() {
  byte buttonState = digitalRead(buttonPin);
  if (buttonState != lastButtonState) {
    lastButtonState = buttonState;
    if (buttonState == LOW) {
      numberKeyPresses++;
      Serial.printf("Taster wurde %u gedrückt!\n", numberKeyPresses);
    }
  }
}