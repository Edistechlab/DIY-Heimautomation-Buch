#include <Bounce2.h> //Bounce2 von Thomas O Fredericks V2.71
const byte buttonPin = 5; //D1
Bounce2::Button button = Bounce2::Button();

void setup() {
  Serial.begin(115200);
  button.attach(buttonPin, INPUT_PULLUP);
  button.interval(30); // 30 ms zum Entprellen
  button.setPressedState(LOW);  // Input = LOW bei gedrückten Taster
}

void loop() {
  button.update();  // Bounce Instance update
  if ( button.pressed() ) {
    Serial.println("Taster ist gedrückt");
  }
}