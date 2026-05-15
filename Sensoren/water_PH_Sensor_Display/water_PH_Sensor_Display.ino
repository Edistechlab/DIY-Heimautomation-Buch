/*
Project:  PH-Sensor & DS18B20 auf TFT Display
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 26.04.2026
Update:   
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
*/

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h>

// 0 = Normal mode, 1 = Start calibration
#define CALIBRATION_MODE 0

// Your calibration solutions (adjust here, e.g., 7.00 instead of 6.86)
const float BUFFER_LOW  = 4.00;
const float BUFFER_MID  = 6.86;
const float BUFFER_HIGH = 9.18;

// TFT Pins
#define TFT_CS    10
#define TFT_DC     9
#define TFT_RST    8
#define TFT_BL     6

// Sensor Pins
#define PH_PIN       A0
#define ONE_WIRE_BUS 2

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// EEPROM Addresses
#define EEPROM_MID  0
#define EEPROM_LOW  4
#define EEPROM_HIGH 8

float vMid = 2.50;
float vLow = 3.00;
float vHigh = 2.00;

int calState = 0;
int stableCounter = 0;
float lastVoltage = 0.0;
int tempStableCounter = 0;
float lastTemp = 0.0;
int lastCalScreen = -1;

float phBuffer[10] = {0};
int phBufferIndex = 0;
bool phBufferFull = false;

void printCentered(String text, int y, int size) {
  tft.setTextSize(size);
  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((tft.width() - w) / 2, y);
  tft.print(text);
}

// Temperature target values for buffers
float getTargetMid(float t)  { return BUFFER_MID - ((t - 25.0) * 0.004); }
float getTargetLow(float t)  { return BUFFER_LOW; }
float getTargetHigh(float t) { return BUFFER_HIGH - ((t - 25.0) * 0.01); }

void setup() {
  Serial.begin(115200);
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);
  
  sensors.begin();

  // Load values from EEPROM
  EEPROM.get(EEPROM_MID, vMid);
  EEPROM.get(EEPROM_LOW, vLow);
  EEPROM.get(EEPROM_HIGH, vHigh);
  
  // Plausibility check (if EEPROM is empty)
  if (isnan(vMid) || vMid < 1.0 || vMid > 5.0) vMid = 2.50;
  if (isnan(vLow) || vLow < 1.0 || vLow > 5.0) vLow = 3.00;
  if (isnan(vHigh) || vHigh < 1.0 || vHigh > 5.0) vHigh = 2.00;

  Serial.print("vMid  (pH 6.86): "); Serial.println(vMid, 3);
  Serial.print("vLow  (pH 4.00): "); Serial.println(vLow, 3);
  Serial.print("vHigh (pH 9.18): "); Serial.println(vHigh, 3);

  if (CALIBRATION_MODE == 0) {
    tft.setTextColor(ST77XX_CYAN); // Light Blue
    printCentered("Edi's Techlab PH-Sensor", 10, 1);
    tft.fillRect(10, 24, tft.width() - 20, 2, ST77XX_CYAN);
    
    tft.setTextColor(ST77XX_WHITE); // Back to White for labels
    tft.setTextSize(2);
    tft.setCursor(20, 45);
    tft.print("pH:");
    tft.setCursor(20, 75);
    tft.print("Temp:");
  }
}

void loop() {
  sensors.requestTemperatures();
  float temp = sensors.getTempCByIndex(0);
  if (temp == DEVICE_DISCONNECTED_C) temp = 25.0; // Fallback on Error

  // Average of 10 measurements
  int phRaw = 0;
  for (int i = 0; i < 10; i++) {
    phRaw += analogRead(PH_PIN);
    delay(10);
  }
  float phVoltage = (phRaw / 10.0) * (5.0 / 1023.0);

  if (CALIBRATION_MODE == 1) {
    runCalibration(phVoltage, temp);
  } else {
    runNormalMode(phVoltage, temp);
  }
  delay(1000); // 1 Hz Update
}

void runNormalMode(float v, float t) {
  float phValue = 0;

  // 3-point interpolation with temp compensation
  if (v > vMid) {
    float slope = (getTargetMid(t) - getTargetLow(t)) / (vMid - vLow);
    phValue = getTargetMid(t) + ((v - vMid) * slope);
  } else {
    float slope = (getTargetHigh(t) - getTargetMid(t)) / (vHigh - vMid);
    phValue = getTargetMid(t) + ((v - vMid) * slope);
  }

  // Gleitender Mittelwert über 10 Messungen
  phBuffer[phBufferIndex] = phValue;
  phBufferIndex = (phBufferIndex + 1) % 10;
  if (phBufferIndex == 0) phBufferFull = true;

  int count = phBufferFull ? 10 : phBufferIndex;
  float phAvg = 0;
  for (int i = 0; i < count; i++) phAvg += phBuffer[i];
  phAvg /= count;

  // Display update (flicker-free)
  tft.fillRect(85, 45, 75, 16, ST77XX_BLACK);
  tft.fillRect(85, 75, 75, 16, ST77XX_BLACK);

  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);

  tft.setCursor(85, 45);
  tft.print(phAvg, 2);

  tft.setCursor(85, 75);
  tft.print(t, 1);
  tft.print(" C");
}

void runCalibration(float v, float t) {
  if (abs(t - lastTemp) < 0.2) tempStableCounter++;
  else tempStableCounter = 0;
  lastTemp = t;

  if (abs(v - lastVoltage) < 0.02) stableCounter++;
  else stableCounter = 0;
  lastVoltage = v;

  bool tempReady = (tempStableCounter >= 10);

  // Screen ID: nur bei Wechsel fillScreen aufrufen
  int screenId;
  if (!tempReady)    screenId = calState;       // 0, 1, 2 je nach nächster Solution
  else if (calState == 0) screenId = 10;
  else                    screenId = 20 + calState;  // 21, 22, 23

  bool newScreen = (screenId != lastCalScreen);
  if (newScreen) {
    tft.fillScreen(ST77XX_BLACK);
    lastCalScreen = screenId;
  }

  // Screen 1: Warten auf stabile Temperatur
  if (!tempReady) {
    stableCounter = 0;
    if (newScreen) {
      String nextPH = (calState == 0) ? String(BUFFER_MID, 2)
                    : (calState == 1) ? String(BUFFER_LOW, 2)
                                      : String(BUFFER_HIGH, 2);
      tft.setTextColor(ST77XX_YELLOW);
      printCentered("CALIBRATION", 5, 1);
      tft.fillRect(0, 17, tft.width(), 1, ST77XX_YELLOW);
      tft.setTextColor(ST77XX_WHITE);
      printCentered("Prepare for:", 24, 1);
      tft.setTextColor(ST77XX_CYAN);
      printCentered("pH " + nextPH + " Solution", 34, 1);
      tft.setTextColor(ST77XX_ORANGE);
      printCentered("Wait for Temp...", 76, 1);
    }
    tft.fillRect(0, 46, tft.width(), 11, ST77XX_BLACK);
    tft.setTextColor(ST77XX_WHITE);
    printCentered("Temp: " + String(t, 1) + " C", 50, 1);
    tft.fillRect(0, 60, tft.width(), 11, ST77XX_BLACK);
    printCentered("Volt: " + String(v, 2) + " V", 64, 1);
    tft.fillRect(0, 86, tft.width(), 11, ST77XX_BLACK);
    tft.setTextColor(ST77XX_ORANGE);
    printCentered(String(tempStableCounter) + " / 10", 90, 1);
    return;
  }

  // Screen 2: Mittelpunkt-Kalibrierung
  if (calState == 0) {
    if (newScreen) {
      tft.setTextColor(ST77XX_YELLOW);
      printCentered("MID CALIBRATION", 5, 1);
      tft.fillRect(0, 17, tft.width(), 1, ST77XX_YELLOW);
      tft.setTextColor(ST77XX_CYAN);
      printCentered("Dip in pH " + String(BUFFER_MID, 2), 74, 1);
    }
    tft.fillRect(0, 22, tft.width(), 11, ST77XX_BLACK);
    tft.setTextColor(ST77XX_WHITE);
    printCentered("Temp: " + String(t, 1) + " C", 26, 1);
    tft.fillRect(0, 36, tft.width(), 11, ST77XX_BLACK);
    printCentered("Volt: " + String(v, 2) + " V", 40, 1);
    tft.fillRect(0, 51, tft.width(), 11, ST77XX_BLACK);
    tft.setTextColor(ST77XX_GREEN);
    printCentered("Volt Stable: " + String(stableCounter) + " / 15", 55, 1);

    if (stableCounter >= 15) {
      vMid = v;
      EEPROM.put(EEPROM_MID, vMid);
      calState = 1;
      tempStableCounter = 0;
      stableCounter = 0;
    }

  // Screen 3: Low-Puffer Kalibrierung
  } else if (calState == 1) {
    if (newScreen) {
      tft.setTextColor(ST77XX_YELLOW);
      printCentered("LOW CALIBRATION", 5, 1);
      tft.fillRect(0, 17, tft.width(), 1, ST77XX_YELLOW);
      tft.setTextColor(ST77XX_CYAN);
      printCentered("Dip in pH " + String(BUFFER_LOW, 2), 74, 1);
    }
    tft.fillRect(0, 22, tft.width(), 11, ST77XX_BLACK);
    tft.setTextColor(ST77XX_WHITE);
    printCentered("Temp: " + String(t, 1) + " C", 26, 1);
    tft.fillRect(0, 36, tft.width(), 11, ST77XX_BLACK);
    printCentered("Volt: " + String(v, 2) + " V", 40, 1);
    bool lowOk = (v > vMid + 0.1);
    if (!lowOk) stableCounter = 0;
    tft.fillRect(0, 51, tft.width(), 11, ST77XX_BLACK);
    if (lowOk) {
      tft.setTextColor(ST77XX_GREEN);
      printCentered("Volt Stable: " + String(stableCounter) + " / 15", 55, 1);
    } else {
      tft.setTextColor(ST77XX_RED);
      printCentered("Wrong solution!", 55, 1);
    }
    if (stableCounter >= 15) {
      vLow = v;
      EEPROM.put(EEPROM_LOW, vLow);
      calState = 2;
      tempStableCounter = 0;
      stableCounter = 0;
    }

  // Screen 4: High-Puffer Kalibrierung
  } else if (calState == 2) {
    if (newScreen) {
      tft.setTextColor(ST77XX_YELLOW);
      printCentered("HIGH CALIBRATION", 5, 1);
      tft.fillRect(0, 17, tft.width(), 1, ST77XX_YELLOW);
      tft.setTextColor(ST77XX_CYAN);
      printCentered("Dip in pH " + String(BUFFER_HIGH, 2), 74, 1);
    }
    tft.fillRect(0, 22, tft.width(), 11, ST77XX_BLACK);
    tft.setTextColor(ST77XX_WHITE);
    printCentered("Temp: " + String(t, 1) + " C", 26, 1);
    tft.fillRect(0, 36, tft.width(), 11, ST77XX_BLACK);
    printCentered("Volt: " + String(v, 2) + " V", 40, 1);
    bool highOk = (v < vMid - 0.1);
    if (!highOk) stableCounter = 0;
    tft.fillRect(0, 51, tft.width(), 11, ST77XX_BLACK);
    if (highOk) {
      tft.setTextColor(ST77XX_GREEN);
      printCentered("Volt Stable: " + String(stableCounter) + " / 15", 55, 1);
    } else {
      tft.setTextColor(ST77XX_RED);
      printCentered("Wrong solution!", 55, 1);
    }
    if (stableCounter >= 15) {
      vHigh = v;
      EEPROM.put(EEPROM_HIGH, vHigh);
      calState = 3;
    }

  // Screen 5: Fertig
  } else if (calState == 3) {
    if (newScreen) {
      tft.setTextColor(ST77XX_GREEN);
      printCentered("DONE!", 40, 2);
      tft.setTextColor(ST77XX_WHITE);
      printCentered("Set CAL_MODE to 0", 70, 1);
    }
  }
}