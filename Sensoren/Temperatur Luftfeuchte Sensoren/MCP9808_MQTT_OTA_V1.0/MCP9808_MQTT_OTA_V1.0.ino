/*
Project:  MCP9808 Temperatur Sensor über MQTT und OTA update
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 23.08.2020
Version:  V1.0
IDE:      Arduino IDE 1.8.13
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V2.7.4 
 - Board: esp32   by Espressif Systems   V1.0.2

Required libraries (sketch -> include library -> manage libraries)
 - Adafruit_MCP9808 library V1.1.2 
 - PubSubClient by Nick ‘O Leary V2.8.0
 - ArduinoOTA by Juraj Andrassy V1.0.5

Wirering for the MCP9808 Sensor:
MCP9808     ESP32       ESP8266     UNO         Mega
VCC         3.3V        3.3V        3.3V        3.3V      
GND         GND         GND         GND         GND
SCL         22          D1/5        A5          21
SDA         21          D2/4        A4          20
A0          3.3V/GND    3.3V/GND    3.3V/GND    3.3V/GND  
A1          3.3V/GND    3.3V/GND    3.3V/GND    3.3V/GND
A2          3.3V/GND    3.3V/GND    3.3V/GND    3.3V/GND  
ALERT       input Pin   input Pin   input Pin   input Pin
*/

#include "Adafruit_MCP9808.h"
#include <PubSubClient.h>
#include <ArduinoOTA.h>

#define wifi_ssid "Your_SSID"
#define wifi_password "Your_Password"
#define mqtt_server "MQTT_Server_IP"
#define mqtt_user "MQTT_username"         
#define mqtt_password "MQTT_PW"
#define ESPHostname "MCP9808_Sensor"

#define temperature_topic "esp01/temperature"
#define inTopic "esp01/inTopic"
#define outTopic "esp01/outTopic"

Adafruit_MCP9808 sensor = Adafruit_MCP9808();
float temp = 0.00;
float tempDiff = 0.01; //Die hohe Auflösung nutzen
int sensorTakt = 2000; //alle 2 Sekunden wird der Sensor ausgelesen

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup() {
  Serial.begin(115200);
  setup_wifi();
   ArduinoOTA.setHostname(ESPHostname);
  // ArduinoOTA.setPassword("admin");
  ArduinoOTA.begin();
  
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  if (!sensor.begin(0x18)) {
    Serial.println("MCP9808 Sensor nicht gefunden, überprüfe die Adresse oder Verkabelung!");
    while (true);
  }
  sensor.setResolution(3);  // 0.0625°C    250 ms
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  ArduinoOTA.handle(); 
  long now = millis();
  if (now - lastMsg > sensorTakt) {   
    lastMsg = now;
    getMCP9808Values();
  }
}

void getMCP9808Values() {
    sensor.wake();   // Sensor aufwecken
    float newTemp = sensor.readTempC();
    if (checkBound(newTemp, temp, tempDiff)) {
      temp = newTemp;
      Serial.print("Temperature:");
      Serial.println(String(temp).c_str());
      client.publish(temperature_topic, String(temp).c_str(), true);
      sensor.shutdown_wake(1); // Sensor schlafen legen; Stromverbrauch ~0.1 mikro Ampere
      delay(200);
    }
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// gesendete MQTT Daten an den Microcontroller auswerten
void callback(char* topic, byte* payload, unsigned int length) {
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  if ((char)payload[0] == '1') {
    //Room for Code
  } else {
    // Room for Code
  }
}

bool checkBound(float newValue, float prevValue, float maxDiff) {
  return !isnan(newValue) &&
  (newValue < prevValue - maxDiff || newValue > prevValue + maxDiff);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP01-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(outTopic, ESPHostname);
      // ... and resubscribe
      client.subscribe(inTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
