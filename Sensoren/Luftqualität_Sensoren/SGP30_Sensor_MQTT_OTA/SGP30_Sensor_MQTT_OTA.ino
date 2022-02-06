/*
Project:  SGP30 VOC I2C-Sensor with MQTT and OTA / Modification from the Adafruit sgp30test code
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 05.02.2022
Version:  V1.0
IDE:      Arduino IDE 1.8.16
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V3.0.2 
 - Board: esp32   by Espressif Systems   V1.0.6

Required libraries (sketch -> include library -> manage libraries)
 - Adafruit_SGP30 library V2.0.0
 - PubSubClient by Nick ‘O Leary V2.8.0
 - ArduinoOTA by Juraj Andrassy V1.0.7  
*/

#include <PubSubClient.h>
#include <ArduinoOTA.h>
//#include <WiFi.h>  // ESP32 only
#include <Wire.h>
#include "Adafruit_SGP30.h"

#define wifi_ssid "Your_SSID"
#define wifi_password "Your_Password"
#define mqtt_server "MQTT_Server_IP"
#define mqtt_user "MQTT_username"         
#define mqtt_password "MQTT_PW"
#define ESPHostname "SGP30_01"
String clientId = "SGP30_01-"; 

#define tvoc_topic "tvoc"
#define tvocBase_topic "tvocBase"
#define eco2_topic "eco2"
#define eco2Base_topic "eco2Base"
#define status_topic "SGP30_01"
#define intopic "intopic"

float tvoc = 100.00;
float eCO2 = 200.00;
float tvocDiff = 1.00;
float eCO2Diff = 1.00;
int counter = 0;
const int sensorTakt = 2000; //alle Sekunden wird der Sensor ausgelesen
long lastMsg = 0;
char msg[50];

WiFiClient espClient;  
PubSubClient client(espClient);

Adafruit_SGP30 sgp;
uint32_t getAbsoluteHumidity(float temperature, float humidity) {
    // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
    const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
    const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity); // [mg/m^3]
    return absoluteHumidityScaled;
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  ArduinoOTA.setHostname(ESPHostname);
  // ArduinoOTA.setPassword("admin");
  ArduinoOTA.begin();
  client.setServer(mqtt_server, 1883); 
  client.setCallback(callback); 

  if (! sgp.begin()){
    Serial.println("Sensor not found :-(");
    while (1);
  }
  //Hier können die Baseline Werte wenn vorhanden eingegeben werden, wenn nicht, diese Zeile auskommentierern
  //sgp.setIAQBaseline(0x7E94, 0x8A51);  // Baseline Werte nach 12h Betrieb vom Serial Monitor entnehmen!
}

void loop() {
  //Hier können die Luftfeuchte und Temperaturwerte für genauere Messwerte eingegeben werden.
  //float temperature = 22.1; // [°C]
  //float humidity = 45.2; // [%RH]
  //sgp.setHumidity(getAbsoluteHumidity(temperature, humidity));

  if (!client.connected()) {
      reconnect();
  }
  client.loop();
  ArduinoOTA.handle(); 
  long now = millis();
  if (now - lastMsg > sensorTakt) {   
    lastMsg = now;
    getSGP30Values();
  }
} 

void getSGP30Values() {
 if (! sgp.IAQmeasure()) {
    Serial.println("Measurement failed");
    return;
 }
 float newTvoc = (sgp.TVOC);
 float newECO2 = (sgp.eCO2);

 if (checkBound(newTvoc, tvoc, tvocDiff)) {
   tvoc = newTvoc;
   Serial.print("TVOC:");
   Serial.println(String(tvoc).c_str());
   client.publish(tvoc_topic, String(tvoc).c_str(), true);
 }

 if (checkBound(newECO2, eCO2, eCO2Diff)) {
   eCO2 = newECO2;
   Serial.print("eCO2:");
   Serial.println(String(eCO2).c_str());
   client.publish(eco2_topic, String(eCO2).c_str(), true);
 }

 counter++;
 if (counter == 30) {
   printBaseline();  
 }
}
  
void printBaseline() {
  uint16_t TVOC_base, eCO2_base;
  if (! sgp.getIAQBaseline(&eCO2_base, &TVOC_base)) {
    Serial.println("Failed to get baseline readings");
    return;
  }
  Serial.print("Baseline Werte: eCO2: 0x"); Serial.print(eCO2_base, HEX);
  Serial.print(" & TVOC: 0x"); Serial.println(TVOC_base, HEX);
  client.publish(eco2Base_topic, String(eCO2_base, HEX).c_str(), true);
  client.publish(tvocBase_topic, String(TVOC_base, HEX).c_str(), true);
  counter = 0;
}

bool checkBound(float newValue, float prevValue, float maxDiff) {
  return !isnan(newValue) &&
  (newValue < prevValue - maxDiff || newValue > prevValue + maxDiff);
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

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
  if (String(topic) == intopic) {
    // Room for Code on an intopic
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(status_topic, ESPHostname);
      // ... and resubscribe
      client.subscribe(intopic);
  } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
