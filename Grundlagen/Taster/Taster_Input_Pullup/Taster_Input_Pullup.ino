const byte buttonPin = 5; //D1

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP); 
}

void loop() {
  Serial.println(digitalRead(buttonPin));
}