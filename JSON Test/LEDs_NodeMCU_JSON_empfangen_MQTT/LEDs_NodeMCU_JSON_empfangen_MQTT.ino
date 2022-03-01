/*
Project:  LEDs am NodeMCU ESP8266 - JSON empfangen
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 28.02.2022
Version:  V1.0
IDE:      Arduino IDE 1.8.16
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V3.0.2 

Required libraries (sketch -> include library -> manage libraries)
 - ArduinoJson Library by Benolt Blanchon V6.19.2
 - PubSubClient by Nick ‘O Leary V2.8.0
 - ArduinoOTA by Juraj Andrassy V1.0.7  
*/

#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>

#define wifi_ssid "Your_SSID"
#define wifi_password "Your_Password"
#define mqtt_server "MQTT_Server_IP"
#define mqtt_user "MQTT_username"         
#define mqtt_password "MQTT_PW"
#define ESPHostname "JSON_Test_Reciver"
#define intopic "JsonTestLED"
#define outtopic "JsonEmpfTestOut"
String clientId = "JSON_Test_Reciver"; 

StaticJsonDocument<100> doc;
const int LEDrotPin = 4;
const int LEDgruenPin = 5;
int Poti;
char msg[50];

WiFiClient espClient;  
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  setup_wifi();
  ArduinoOTA.setHostname(ESPHostname);
  // ArduinoOTA.setPassword("admin");
  ArduinoOTA.begin();
  client.setServer(mqtt_server, 1883); 
  client.setCallback(callback);
  pinMode(LEDrotPin, OUTPUT);
  pinMode(LEDgruenPin, OUTPUT);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  ArduinoOTA.handle();
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
    String meinJson = messageTemp;
    DeserializationError error = deserializeJson(doc, meinJson);
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }
    bool LEDRot = doc["ledRot"];
    digitalWrite(LEDrotPin, LEDRot);
    bool LEDGruen = doc["ledGruen"];
    digitalWrite(LEDgruenPin, LEDGruen);
    Poti = doc["poti"];
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
      client.publish(outtopic, ESPHostname);
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
