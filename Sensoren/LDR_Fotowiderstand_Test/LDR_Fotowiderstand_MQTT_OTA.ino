/*
Project:  LDR Fotowiderstand Sensor über MQTT und OTA update
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 01.12.2020
Version:  V1.0
IDE:      Arduino IDE 1.8.13
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V2.7.4 
 - Board: esp32   by Espressif Systems   V1.0.4

Required libraries (sketch -> include library -> manage libraries)
 - PubSubClient by Nick ‘O Leary V2.8.0
 - ArduinoOTA by Juraj Andrassy V1.0.5
*/

#include <PubSubClient.h>
#include <ArduinoOTA.h>
//#include <WiFi.h>         // Wird nur beim ESP32 benötigt, beim ESP8266 auskommentieren

#define wifi_ssid "Your_SSID"
#define wifi_password "Your_Password"
#define mqtt_server "MQTT_Server_IP"
#define mqtt_user "MQTT_username"         
#define mqtt_password "MQTT_PW"
#define ESPHostname "LDR_Sensor"

int LDRpin = 17;          //Pin A0 (17) - ADC am NodeMCU an dem der LDR angeschlossen ist.
int sensorTakt = 5000;    //alle 5 Sekunden wird der Sensor ausgelesen
char charPayload[50];

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];

void setup() {
  Serial.begin(115200);
  setup_wifi();
   ArduinoOTA.setHostname(ESPHostname);
  // ArduinoOTA.setPassword("admin");
  ArduinoOTA.begin();
  
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
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
    getLDRValues();
  }
} 

void getLDRValues() {
  int LDRvalue = analogRead(LDRpin);
  Serial.print("Analog LDR Value: ");
  Serial.println(String(LDRvalue).c_str());
  client.publish(ESPHostname"/LDR", String(LDRvalue).c_str(), true);
  delay(200);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);
  WiFi.hostname(ESPHostname);
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

// empfangene MQTT Daten auswerten
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived: ");
  String newTopic = topic;
  Serial.print(topic);
  payload[length] = '\0';
  String newPayload = String((char *)payload);
  int intPayload = newPayload.toInt();
  Serial.print(" - ");
  Serial.println(newPayload);
  Serial.println();
  newPayload.toCharArray(charPayload, newPayload.length() + 1); 
  
  if (newTopic == ESPHostname"/inTopic") {
   Serial.println("Ich werde ausgeführt im inTopic");    //Dein Code
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
    String clientId = ESPHostname;
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(ESPHostname"/status", "online");
      // ... and resubscribe
      client.subscribe(ESPHostname"/inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
