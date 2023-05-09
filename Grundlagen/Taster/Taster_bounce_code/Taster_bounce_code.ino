const byte buttonPin = 5; //D1
byte lastButtonState = LOW;
const int debounceTime = 30; // millis
int numberKeyPresses = 0;
unsigned long lastButtonTimeChange = 0;

void setup() {
    Serial.begin(115200);
    pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
    if (millis() - lastButtonTimeChange > debounceTime) {
      byte buttonState = digitalRead(buttonPin);
      if (buttonState != lastButtonState) {
        lastButtonTimeChange = millis();
        lastButtonState = buttonState;
      if (buttonState == LOW) {
        numberKeyPresses++;
        Serial.printf("Taster wurde %u gedrückt!\n", numberKeyPresses);
      }
    }
  }
}