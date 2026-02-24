/*
===============================================================
Project:    HX711 Gas Bottle Scale (ESP32 + MQTT + Deep Sleep)
Author:     Thomas Edlinger
Website:    https://www.edistechlab.com
Created:    24.02.2026
Version:    1.0.0
License:    MIT (recommended for public release)

Description:
---------------------------------------------------------------
Low-power IoT gas bottle scale based on an ESP32-C3 and HX711.
The system measures the total bottle weight, calculates the 
fill level, and publishes all values via MQTT.

Features:
- HX711 load cell support
- MQTT communication (Home Assistant compatible)
- Tare function via MQTT
- 1 kg reference calibration via MQTT
- Configurable bottle parameters
- 50 g change filter to reduce MQTT traffic
- Deep Sleep power saving mode
- Persistent storage (Preferences/NVS)

Hardware:
---------------------------------------------------------------
- ESP32-C3 super mini (Arduino Nano ESP32 compatible)
- HX711 load cell amplifier
- 4-wire load cell
- 3.3V logic
- Optional battery operation

Required Board Package:
---------------------------------------------------------------
ESP32 by Espressif Systems v3.3.7

Required Libraries:
---------------------------------------------------------------
- HX711 Arduino Library by Bogdan Necula (>= 0.7.5)
- PubSubClient by Nick O'Leary (>= 2.8)

Notes:
---------------------------------------------------------------
WiFi and MQTT credentials are stored in secrets.h
and are not part of the public repository.

===============================================================
*/

#include <WiFi.h>
#include <PubSubClient.h>
#include <HX711.h>
#include <Preferences.h>
#include "secrets.h"

/* ============================================================
   HARDWARE CONFIGURATION
   ============================================================ */

// HX711 Anschluss (GPIO Pins des ESP32-C3)
#define HX_DT  4    // Data Pin (DT) vom HX711
#define HX_SCK 5    // Clock Pin (SCK) vom HX711

/* ============================================================
   GAS BOTTLE CONFIGURATION
   ============================================================ */

// Leergewicht der Gasflasche in kg (inkl. Stahlflasche)
#define BOTTLE_EMPTY_KG 16.5

// Gesamtgewicht bei 100% Füllstand in kg
#define BOTTLE_FULL_KG  31.0

/* ============================================================
   CALIBRATION
   ============================================================ */

// Referenzgewicht für Kalibrierung (z.B. 1.000 kg Prüfgewicht)
// Wird verwendet um den Scale-Faktor zu berechnen
#define CAL_REF_KG 1.000

/* ============================================================
   MEASUREMENT TIMING
   ============================================================ */

// Zeit zwischen zwei Messungen während Wachphase (Millisekunden)
#define MEASURE_INTERVAL_MS 8000   // 8 Sekunden

// Gesamte Wachzeit bevor Deep Sleep aktiviert wird
#define MEASURE_WINDOW_MS   60000  // 60 Sekunden

/* ============================================================
   DETECTION & FILTERING
   ============================================================ */

// Mindestgewicht zur Erkennung einer Flasche
// Verhindert Fehltrigger durch Rauschen oder leichte Last
#define BOTTLE_THRESHOLD_KG 2.0

// Minimale Gewichtsänderung bevor neuer Wert via MQTT gesendet wird
// Reduziert unnötigen Traffic (z.B. 0.05 kg = 50 g)
#define CHANGE_THRESHOLD_KG 0.05

/* ============================================================
   POWER MANAGEMENT
   ============================================================ */

// Deep-Sleep Dauer wenn KEINE Flasche erkannt wurde
#define SLEEP_EMPTY_MIN   5   // Minuten

// Deep-Sleep Dauer wenn Flasche erkannt wurde
#define SLEEP_BOTTLE_MIN 30   // Minuten

WiFiClient espClient;
PubSubClient mqtt(espClient);
HX711 scale;
Preferences prefs;

long tare_offset = 0;
float scale_factor = 10000.0;

float lastPublishedWeight = -999;

unsigned long wakeStart;
unsigned long lastMeasure;

/* ================= MQTT Topics ================= */

#define T_WEIGHT  "gasbottle/weight"
#define T_FILL    "gasbottle/fill_level"
#define T_STATUS  "gasbottle/status"
#define T_INFO    "gasbottle/info"
#define T_CMD     "gasbottle/command"
#define T_AVAIL   "gasbottle/availability"

/* ================================================= */

enum Mode {
  MODE_NORMAL,
  MODE_TARE,
  MODE_CAL
};

Mode currentMode = MODE_NORMAL;

/* ================= UTIL ================= */

void publishInfo(String msg) {
  Serial.println("[INFO] " + msg);
  mqtt.publish(T_INFO, msg.c_str(), true);
}

void connectWiFi() {

  Serial.println("[WIFI] Verbinde...");
  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n[WIFI] Verbunden");
}

void connectMQTT() {

  while (!mqtt.connected()) {

    Serial.println("[MQTT] Verbinde...");

    if (mqtt.connect(
          "GasBottleScale",
          mqtt_user,
          mqtt_password,
          T_AVAIL,
          0,
          true,
          "offline")) {

      Serial.println("[MQTT] Verbunden");

      mqtt.publish(T_AVAIL, "online", true);
      mqtt.subscribe(T_CMD);

    } else {
      delay(2000);
    }
  }
}

long readRawAverage(int samples = 15) {
  long sum = 0;
  for (int i = 0; i < samples; i++) {
    while (!scale.is_ready());
    sum += scale.read();
    delay(40);
  }
  return sum / samples;
}

float rawToKg(long raw) {
  float kg = (tare_offset - raw) / scale_factor;
  if (kg < 0.02) kg = 0;
  return kg;
}

/* ================= TARE ================= */

void performTare() {

  currentMode = MODE_TARE;

  publishInfo("TARE – Waage muss leer sein");

  for (int i = 10; i > 0; i--) {
    publishInfo("Tare in " + String(i) + " Sekunden");
    delay(1000);
  }

  tare_offset = readRawAverage(25);
  prefs.putLong("tare_offset", tare_offset);

  publishInfo("Tare abgeschlossen");

  currentMode = MODE_NORMAL;
  wakeStart = millis();
}

/* ================= CALIBRATION ================= */

void performCalibration() {

  currentMode = MODE_CAL;

  publishInfo("Kalibrierung gestartet");
  publishInfo("Waage leer lassen...");
  delay(5000);

  tare_offset = readRawAverage(25);
  prefs.putLong("tare_offset", tare_offset);

  publishInfo("1.000 kg Referenz auflegen");
  delay(10000);

  long raw = readRawAverage(25);
  long diff = tare_offset - raw;

  scale_factor = diff / CAL_REF_KG;
  prefs.putFloat("scale_factor", scale_factor);

  publishInfo("Kalibrierung abgeschlossen");

  currentMode = MODE_NORMAL;
  wakeStart = millis();
}

/* ================= MQTT CALLBACK ================= */

void callback(char* topic, byte* payload, unsigned int length) {

  String msg;
  for (int i = 0; i < length; i++)
    msg += (char)payload[i];

  Serial.println("[CMD] " + msg);

  if (msg == "TARE") performTare();
  if (msg == "CALIBRATE") performCalibration();
}

/* ================= SETUP ================= */

void setup() {

  Serial.begin(115200);
  delay(1000);

  prefs.begin("gaswaage", false);
  tare_offset = prefs.getLong("tare_offset", 0);
  scale_factor = prefs.getFloat("scale_factor", 10000.0);

  scale.begin(HX_DT, HX_SCK);

  connectWiFi();

  mqtt.setServer(mqtt_broker, mqtt_port);
  mqtt.setCallback(callback);
  connectMQTT();

  publishInfo("System wach");

  wakeStart = millis();
  lastMeasure = 0;
}

/* ================= LOOP ================= */

void loop() {

  mqtt.loop();

  if (currentMode != MODE_NORMAL)
    return;

  unsigned long now = millis();

  /* ===== MESSUNG ===== */

  if (now - lastMeasure >= MEASURE_INTERVAL_MS) {

    lastMeasure = now;

    long raw = readRawAverage();
    float kg = rawToKg(raw);
    float kgRounded = round(kg * 100.0) / 100.0;

    Serial.println("---------------");
    Serial.print("RAW: "); Serial.println(raw);
    Serial.print("KG:  "); Serial.println(kgRounded, 2);
    Serial.println("---------------");

    bool bottleDetected = false;
    float fill = 0;
    const char* status = "NO_BOTTLE";

    if (kgRounded > BOTTLE_THRESHOLD_KG) {
      bottleDetected = true;
      status = "BOTTLE";

      float gas = kgRounded - BOTTLE_EMPTY_KG;
      float maxGas = BOTTLE_FULL_KG - BOTTLE_EMPTY_KG;
      fill = (gas / maxGas) * 100.0;
      fill = constrain(fill, 0, 100);
    }

    if (abs(kgRounded - lastPublishedWeight) >= CHANGE_THRESHOLD_KG) {

      lastPublishedWeight = kgRounded;

      mqtt.publish(T_WEIGHT, String(kgRounded, 2).c_str(), true);
      mqtt.publish(T_FILL, String(fill, 1).c_str(), true);
      mqtt.publish(T_STATUS, status, true);
    }
  }

  /* ===== SLEEP ===== */

  if (now - wakeStart >= MEASURE_WINDOW_MS) {

    bool bottlePresent = lastPublishedWeight > BOTTLE_THRESHOLD_KG;

    int sleepMinutes =
      bottlePresent ? SLEEP_BOTTLE_MIN : SLEEP_EMPTY_MIN;

    publishInfo("Deep Sleep für " + String(sleepMinutes) + " Minuten");

    mqtt.publish(T_AVAIL, "offline", true);
    mqtt.loop();
    delay(1000);

    mqtt.disconnect();
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);

    esp_sleep_enable_timer_wakeup(
      (uint64_t)sleepMinutes * 60ULL * 1000000ULL);

    esp_deep_sleep_start();
  }
}