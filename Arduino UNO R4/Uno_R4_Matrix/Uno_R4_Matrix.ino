#include "Arduino_LED_Matrix.h"
ArduinoLEDMatrix matrix;

#define ROWS 8
#define COLUMNS 12

uint8_t techlab[ROWS][COLUMNS] = {
  { 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0 },
  { 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0 },
  { 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0 },
  { 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 0 },
  { 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0 },
  { 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0 },
  { 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0 }
};

uint8_t clearframe[ROWS][COLUMNS] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

void setup() {
  Serial.begin(115200);
  matrix.begin();
}

void loop() {
  matrix.renderBitmap(techlab, ROWS, COLUMNS);
  delay(3000);
  matrix.renderBitmap(clearframe, ROWS, COLUMNS);
  delay(2000);
}