/*
Project:  ORP-Sensor with DS18B20 and TFT Display
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 15.05.2026
Update:   21.07.2026
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

ORP CALIBRATION INFO:
  ORP uses a single-point offset calibration (no K-value needed).
  Common buffer solutions: 225 mV (Zobell), 256 mV, 468 mV, 650 mV
  The calibration saves an mV offset to EEPROM.

CALIBRATION MODES:
  0 = Normal measurement
  1 = Calibration with ORP buffer solution

HOW TO CALIBRATE:
  1. Set REF_ORP_MV to match your buffer solution (225, 256, 468, or 650 mV)
  2. Set CALIBRATION_MODE to 1 and upload
  3. Wait for temperature to stabilise (shown on display)
  4. Dip the ORP probe into the buffer solution
  5. Wait for the reading to stabilise — "DONE!" appears automatically
  6. Set CALIBRATION_MODE to 0 and re-upload
*/

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h>

// 0 = Measure  |  1 = Calibrate with ORP buffer solution
#define CALIBRATION_MODE  0

// ── Reference value of your calibration buffer solution (in mV) ───────────────
//   225  →  Zobell solution (most common)
//   256  →  own buffer solution
//   468  →  standard ORP buffer
//   650  →  high-range buffer
#define REF_ORP_MV  256

// ── Op-amp gain factor (1.037 for most ORP modules) ────────────────────────────
// Negate (e.g. -1.037) if your readings are inverted
#define ORP_GAIN  1.037

// Custom colors
#define ST77XX_PURPLE 0x8010

// TFT Pins
#define TFT_CS    10
#define TFT_DC     9
#define TFT_RST    8
#define TFT_BL     6

// Sensor Pins
#define ORP_PIN      A2
#define ONE_WIRE_BUS 2

// EEPROM address map:  pH = 0–11  |  EC = 12–15  |  ORP = 16–19
#define EEPROM_ORP_ADDR  16

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

float orpOffset         = 0.0;
int   stableCounter     = 0;
int   tempStableCounter = 0;
float lastVoltage       = 0.0;
float lastTemp          = 0.0;
bool  calDone           = false;
int   calState          = -1;  // -1 = not yet initialised, 0 = wait temp, 1 = stabilise, 2 = done

// ── Centre text on full display ───────────────────────────────────────────────
void printCentered(String text, int y, int size) {
  tft.setTextSize(size);
  int16_t x1, y1; uint16_t w, h;
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((tft.width() - w) / 2, y);
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

  EEPROM.get(EEPROM_ORP_ADDR, orpOffset);
  if (isnan(orpOffset) || orpOffset < -2000.0 || orpOffset > 2000.0) orpOffset = 0.0;

  if (CALIBRATION_MODE == 0) {
    // ── Static UI (drawn once) ────────────────────────────────────────────
    tft.setTextColor(ST77XX_PURPLE);
    printCentered("ORP Sensor", 4, 1);
    tft.fillRect(10, 16, tft.width() - 20, 1, ST77XX_PURPLE);

    tft.setTextColor(ST77XX_WHITE);
    printCentered("ORP", 22, 1);

    tft.fillRect(10, 68, tft.width() - 20, 1, ST77XX_WHITE);

    tft.setTextColor(ST77XX_YELLOW);
    printCentered("Temperature", 76, 1);
  }
}

void loop() {
  sensors.requestTemperatures();
  float temp = sensors.getTempCByIndex(0);
  if (temp == DEVICE_DISCONNECTED_C) temp = 25.0;

  int orpRaw = 0;
  for (int i = 0; i < 10; i++) {
    orpRaw += analogRead(ORP_PIN);
    delay(10);
  }
  float voltage = (orpRaw / 10.0) * (5.0 / 1023.0);

  if (CALIBRATION_MODE == 0) runMeasure(voltage, temp);
  else                        runCalibration(voltage, temp);

  delay(1000);
}

// ── Normal measurement ────────────────────────────────────────────────────────
void runMeasure(float v, float t) {
  int orpMV = (int)(((2.5 - v) / ORP_GAIN) * 1000.0 + orpOffset);

  // ORP value — green = positive (oxidising), red = negative (reducing)
  tft.fillRect(0, 30, tft.width(), 36, ST77XX_BLACK);
  tft.setTextColor(orpMV >= 0 ? ST77XX_GREEN : ST77XX_RED);
  printCentered(String(orpMV), 32, 3);
  tft.setTextColor(ST77XX_WHITE);
  printCentered("mV", 56, 1);

  // Temperature
  tft.fillRect(0, 84, tft.width(), 26, ST77XX_BLACK);
  tft.setTextColor(ST77XX_YELLOW);
  printCentered(String(t, 1) + " C", 88, 2);

  Serial.print("ORP: ");   Serial.print(orpMV);
  Serial.print(" mV  |  Temp: "); Serial.print(t, 1);
  Serial.println(" C");
}

// ── Calibration ───────────────────────────────────────────────────────────────
void runCalibration(float v, float t) {
  if (calDone) return;

  float rawORP = ((2.5 - v) / ORP_GAIN) * 1000.0;

  if (abs(t - lastTemp) < 0.2) tempStableCounter++;
  else                          tempStableCounter = 0;
  lastTemp = t;

  // While waiting for temp, keep resetting the ORP counter
  if (tempStableCounter < 10) stableCounter = 0;
  else if (abs(v - lastVoltage) < 0.02) stableCounter++;
  else                                    stableCounter = 0;
  lastVoltage = v;

  int newState = (tempStableCounter < 10) ? 0 : (stableCounter < 15) ? 1 : 2;

  // ── Full redraw only on state change ──────────────────────────────────
  if (newState != calState) {
    calState = newState;
    tft.fillScreen(ST77XX_BLACK);

    tft.setTextColor(ST77XX_PURPLE);
    printCentered("ORP CALIBRATION", 4, 1);
    tft.fillRect(0, 15, tft.width(), 1, ST77XX_PURPLE);

    if (calState == 0) {
      tft.setTextColor(ST77XX_ORANGE);
      printCentered("Waiting for temp...", 56, 1);
    } else if (calState == 1) {
      tft.setTextColor(ST77XX_WHITE);
      printCentered("Dip probe into", 22, 1);
      tft.setTextColor(ST77XX_CYAN);
      printCentered(String(REF_ORP_MV) + " mV buffer", 34, 1);
      tft.setTextColor(ST77XX_GREEN);
      printCentered("Stabilizing:", 80, 1);
    }
  }

  // ── Dynamic updates (only changing values) ────────────────────────────
  if (calState == 0) {
    tft.fillRect(0, 22, tft.width(), 26, ST77XX_BLACK);
    tft.setTextColor(ST77XX_WHITE);
    printCentered("Temp: " + String(t, 1) + " C", 24, 1);
    printCentered("ORP:  " + String((int)rawORP) + " mV", 36, 1);
    tft.fillRect(0, 68, tft.width(), 10, ST77XX_BLACK);
    tft.setTextColor(ST77XX_ORANGE);
    printCentered(String(tempStableCounter) + " / 10", 70, 1);

  } else if (calState == 1) {
    tft.fillRect(0, 48, tft.width(), 24, ST77XX_BLACK);
    tft.setTextColor(ST77XX_WHITE);
    printCentered("Temp: " + String(t, 1) + " C", 50, 1);
    printCentered("ORP:  " + String((int)rawORP) + " mV", 62, 1);
    tft.fillRect(0, 92, tft.width(), 10, ST77XX_BLACK);
    tft.setTextColor(ST77XX_GREEN);
    printCentered(String(stableCounter) + " / 15", 93, 1);

  } else {
    // ── Done: calculate, save, display ───────────────────────────────
    orpOffset = (float)REF_ORP_MV - rawORP;
    EEPROM.put(EEPROM_ORP_ADDR, orpOffset);
    calDone = true;

    tft.setTextColor(ST77XX_GREEN);
    printCentered("DONE!", 18, 2);
    tft.fillRect(10, 36, tft.width() - 20, 1, ST77XX_GREEN);

    tft.setTextColor(ST77XX_WHITE);
    printCentered("Offset:", 44, 1);
    tft.setTextColor(ST77XX_CYAN);
    printCentered(String(orpOffset, 1) + " mV", 56, 2);

    tft.setTextColor(ST77XX_WHITE);
    printCentered("Set CAL_MODE to 0", 90, 1);
    printCentered("and re-upload.", 102, 1);

    Serial.print("Calibration complete. Offset = ");
    Serial.print(orpOffset, 2);
    Serial.println(" mV");
  }
}
