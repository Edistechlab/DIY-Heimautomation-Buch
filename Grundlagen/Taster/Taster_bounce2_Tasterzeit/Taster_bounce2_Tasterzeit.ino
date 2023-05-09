#include <Bounce2.h> //Bounce2 von Thomas O Fredericks V2.71
const byte buttonPin = 5; //D1
Bounce2::Button button = Bounce2::Button();

void setup() {
  Serial.begin(115200);
  button.attach(buttonPin, INPUT_PULLUP);
  button.interval(10); // 10 ms zum Entprellen
}

void loop() {
  button.update();  // Bounce Instance update
  if ( button.read() == LOW && button.currentDuration() > 1000 ) {
    Serial.println("Taster ist schon über 1 Sek. gedrückt! ");
  }
  if ( button.rose() ) {
    Serial.print("Der Taster wurde so viele Mili Sek. gedrückt: ");
    Serial.println(button.previousDuration());
  }
}