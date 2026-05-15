/*
Project:  EC- & TDS-Sensor mit DS18B20 auf TFT Display
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 15.05.2026
Update:   15.05.2026
Version:  V1.0
IDE:      Arduino IDE 2.3.5

Required Board (Tools -> Board -> Boards Manager...)
- Microcontroller: Arduino Uno
- Board: Arduino AVR Boards

Required libraries (Tools -> manage libraries)
- Adafruit GFX Library by Adafruit V1.12.6
- Adafruit ST7735 and ST7789 Library by Adafruit V1.11.0
- OneWire by Jim Studt V2.3.8
- DallasTemperature by Miles Burton V4.0.6

CALIBRATION MODES:
  0 = Normal measurement
  1 = Calibration with 1.413 mS/cm reference solution
  2 = Calibration with your own reference values (see REF_EC_USER / REF_TDS_USER below)

HOW TO CALIBRATE (Mode 2):
  1. Measure your water with an external meter, enter the values below
  2. Set CALIBRATION_MODE to 2 and upload
  3. Dip the EC sensor into the same water
  4. Wait for "DONE!" — note the K-Value shown on display
  5. Set CALIBRATION_MODE to 0 and re-upload
*/

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h>

// 0 = Measure  |  1 = Cal with 1.413 mS/cm solution  |  2 = Cal with own reference
#define CALIBRATION_MODE  0

// ── Mode 1: standard calibration solution ─────────────────────────────────────
#define REF_EC_CAL    1.413   // mS/cm

// ── Mode 2: your own reference values from an external meter ──────────────────
// Last measurement: EC = 364 µS/cm | TDS = 182 ppm | Temp = 32.7 °C
#define REF_EC_USER   0.364   // mS/cm  (= 364 µS/cm)
#define REF_TDS_USER  182     // ppm    (display reference only)
#define REF_TEMP_USER 32.7    // °C     (display reference only)

// ── TDS conversion factor: ppm = µS/cm × factor ───────────────────────────────
// 0.5 = general / NaCl  |  0.64 = KCl  |  0.67 = mixed ions
#define TDS_FACTOR  0.5

// TFT Pins
#define TFT_CS    10
#define TFT_DC     9
#define TFT_RST    8
#define TFT_BL     6

// Sensor Pins
#define EC_PIN       A1
#define ONE_WIRE_BUS 2

// EEPROM — starts at 12 to not overlap with pH calibration (addresses 0–11)
#define EEPROM_K_ADDR  12

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

float kValue            = 2.9;
int   stableCounter     = 0;
int   tempStableCounter = 0;
float lastVoltage       = 0.0;
float lastTemp          = 0.0;
bool  calDone           = false;
int   calState          = -1;   // -1 = not yet initialised, 0 = wait temp, 1 = stabilise, 2 = done

// ── Centre text on full display ───────────────────────────────────────────────
void printCentered(String text, int y, int size) {
  tft.setTextSize(size);
  int16_t x1, y1; uint16_t w, h;
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((tft.width() - w) / 2, y);
  tft.print(text);
}

// ── Centre text within a horizontal zone ──────────────────────────────────────
void printInZone(String text, int y, int size, int xStart, int xEnd) {
  tft.setTextSize(size);
  int16_t x1, y1; uint16_t w, h;
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  int xPos = xStart + ((xEnd - xStart) - (int)w) / 2;
  tft.setCursor(max(xStart, xPos), y);
  tft.print(text);
}

void setup() {
  Serial.begin(9600);
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);

  sensors.begin();

  EEPROM.get(EEPROM_K_ADDR, kValue);
  if (isnan(kValue) || kValue < 0.1 || kValue > 20.0) kValue = 2.9;

  if (CALIBRATION_MODE == 0) {
    // ── Static UI (drawn once) ────────────────────────────────────────────
    tft.setTextColor(ST77XX_CYAN);
    printCentered("EC & TDS Sensor", 4, 1);
    tft.fillRect(10, 16, tft.width() - 20, 1, ST77XX_CYAN);

    tft.setTextColor(ST77XX_WHITE);
    printInZone("EC",  22, 1, 0,  79);
    printInZone("TDS", 22, 1, 80, 160);

    tft.fillRect(79, 17, 1, 51, ST77XX_WHITE);    // vertical column divider
    tft.fillRect(10, 68, tft.width() - 20, 1, ST77XX_WHITE);  // horizontal divider

    tft.setTextColor(ST77XX_YELLOW);
    printCentered("Temperature", 76, 1);           // label shifted down slightly
  }
}

void loop() {
  sensors.requestTemperatures();
  float temp = sensors.getTempCByIndex(0);
  if (temp == DEVICE_DISCONNECTED_C) temp = 25.0;

  int ecRaw = 0;
  for (int i = 0; i < 10; i++) {
    ecRaw += analogRead(EC_PIN);
    delay(10);
  }
  float voltage = (ecRaw / 10.0) * (5.0 / 1023.0);

  if (CALIBRATION_MODE == 0) runMeasure(voltage, temp);
  else                        runCalibration(voltage, temp);

  delay(1000);
}

// ── Normal measurement ────────────────────────────────────────────────────────
void runMeasure(float v, float t) {
  float tempCoeff = 1.0 + 0.02 * (t - 25.0);
  int   ecUS      = (int)((v * kValue * 1000.0) / tempCoeff);  // µS/cm
  int   tdsValue  = (int)(ecUS * TDS_FACTOR);                  // ppm

  // EC value — left column (x 0–78)
  tft.fillRect(0, 30, 78, 38, ST77XX_BLACK);
  tft.setTextColor(ST77XX_GREEN);
  printInZone(String(ecUS), 32, 2, 0, 79);
  tft.setTextColor(ST77XX_WHITE);
  printInZone("uS/cm", 52, 1, 0, 79);

  // TDS value — right column (x 80–159)
  tft.fillRect(80, 30, 80, 38, ST77XX_BLACK);
  tft.setTextColor(ST77XX_CYAN);
  printInZone(String(tdsValue), 32, 2, 80, 160);
  tft.setTextColor(ST77XX_WHITE);
  printInZone("ppm", 52, 1, 80, 160);

  // Temperature — shifted down slightly
  tft.fillRect(0, 88, tft.width(), 22, ST77XX_BLACK);
  tft.setTextColor(ST77XX_YELLOW);
  printCentered(String(t, 1) + " C", 90, 2);

  Serial.print("EC: ");    Serial.print(ecUS);
  Serial.print(" uS/cm  |  TDS: "); Serial.print(tdsValue);
  Serial.print(" ppm  |  Temp: "); Serial.print(t, 1);
  Serial.println(" C");
}

// ── Calibration (mode 1 & 2) ──────────────────────────────────────────────────
void runCalibration(float v, float t) {
  if (calDone) return;

  float refEC = (CALIBRATION_MODE == 1) ? REF_EC_CAL : REF_EC_USER;

  if (abs(t - lastTemp) < 0.2)     tempStableCounter++;
  else                              tempStableCounter = 0;
  lastTemp = t;

  // While waiting for temp, keep resetting the voltage counter
  if (tempStableCounter < 10) stableCounter = 0;
  else if (abs(v - lastVoltage) < 0.01) stableCounter++;
  else                                   stableCounter = 0;
  lastVoltage = v;

  int newState = (tempStableCounter < 10) ? 0 : (stableCounter < 15) ? 1 : 2;

  // ── Full redraw only on state change ──────────────────────────────────
  if (newState != calState) {
    calState = newState;
    tft.fillScreen(ST77XX_BLACK);

    tft.setTextColor(ST77XX_YELLOW);
    printCentered(CALIBRATION_MODE == 1 ? "CAL - MODE 1" : "CAL - MODE 2", 4, 1);
    tft.fillRect(0, 15, tft.width(), 1, ST77XX_YELLOW);

    if (calState == 0) {
      tft.setTextColor(ST77XX_ORANGE);
      printCentered("Waiting for temp...", 56, 1);
    } else if (calState == 1) {
      if (CALIBRATION_MODE == 1) {
        tft.setTextColor(ST77XX_WHITE);
        printCentered("Dip sensor into", 24, 1);
        tft.setTextColor(ST77XX_CYAN);
        printCentered("1.413 mS/cm solution", 36, 1);
      } else {
        tft.setTextColor(ST77XX_CYAN);
        printCentered("Ref: " + String((int)(REF_EC_USER * 1000)) +
                      " uS / " + String(REF_TDS_USER) + " ppm", 24, 1);
        tft.setTextColor(ST77XX_WHITE);
        printCentered("Dip into reference water", 36, 1);
      }
      tft.setTextColor(ST77XX_GREEN);
      printCentered("Stabilizing:", 82, 1);
    }
  }

  // ── Dynamic updates (only changing values, no full redraw) ────────────
  if (calState == 0) {
    tft.fillRect(0, 22, tft.width(), 26, ST77XX_BLACK);
    tft.setTextColor(ST77XX_WHITE);
    printCentered("Temp: " + String(t, 1) + " C", 24, 1);
    printCentered("Volt: " + String(v, 3) + " V", 36, 1);
    tft.fillRect(0, 68, tft.width(), 10, ST77XX_BLACK);
    tft.setTextColor(ST77XX_ORANGE);
    printCentered(String(tempStableCounter) + " / 10", 70, 1);

  } else if (calState == 1) {
    tft.fillRect(0, 50, tft.width(), 22, ST77XX_BLACK);
    tft.setTextColor(ST77XX_WHITE);
    printCentered("Temp: " + String(t, 1) + " C", 52, 1);
    printCentered("Volt: " + String(v, 3) + " V", 64, 1);
    tft.fillRect(0, 92, tft.width(), 10, ST77XX_BLACK);
    tft.setTextColor(ST77XX_GREEN);
    printCentered(String(stableCounter) + " / 15", 93, 1);

  } else {
    // ── Done: calculate, save, display ───────────────────────────────
    float tempCoeff = 1.0 + 0.02 * (t - 25.0);
    kValue = (refEC * tempCoeff) / v;
    EEPROM.put(EEPROM_K_ADDR, kValue);
    calDone = true;

    tft.setTextColor(ST77XX_GREEN);
    printCentered("DONE!", 18, 2);
    tft.fillRect(10, 36, tft.width() - 20, 1, ST77XX_GREEN);

    tft.setTextColor(ST77XX_WHITE);
    printCentered("K-Value:", 44, 1);
    tft.setTextColor(ST77XX_CYAN);
    printCentered(String(kValue, 3), 56, 3);

    tft.setTextColor(ST77XX_WHITE);
    printCentered("Set CAL_MODE to 0", 96, 1);
    printCentered("and re-upload.", 108, 1);

    Serial.print("Calibration complete. K = ");
    Serial.println(kValue, 4);
  }
}
