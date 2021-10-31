/*
Project:  DS18B20 Sensor - with MQTT und OTA update
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 30.10.2021
Version:  V1.0
IDE:      Arduino IDE 1.8.16
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V3.0.2 
 - Board: esp32   by Espressif Systems   V1.0.6

Required libraries (sketch -> include library -> manage libraries)
 - OneWire libary V2.3.5 by Jim Studt
 - DallasTemperature V3.9.0 by Miles Burton
 - PubSubClient by Nick ‘O Leary V2.8.0
 - ArduinoOTA by Juraj Andrassy V1.0.5

Connect the DS18B20 Sensor:
Sensor         ESP32    ESP8266    UNO      Mega
VDD            3,3V     3,3V       5V       5V
DQ / 4,7kOhm   4        4          4        4
GND            GND      GND        GND      GND             
*/

#include <OneWire.h>
#include <DallasTemperature.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include <WiFi.h>  // ESP32 only

#define wifi_ssid "Your_SSID"
#define wifi_password "Your_Password"
#define mqtt_server "MQTT_Server_IP"
#define mqtt_user "MQTT_username"         
#define mqtt_password "MQTT_PW"
#define ESPHostname "Heater_Sensor"
String clientId = "Heater_Sensor-"; 

#define inTopic "HeaterSensorIn"
#define outTopic "setCurrent"

const int oneWireBus = 4;   //GPIO Pin 1-Wire Bus
const int sensorTakt = 10000; //alle 10 Sekunden wird der Sensor ausgelesen
long lastMsg = 0;
char msg[50];
float tempSensor1;
uint8_t sensor1[8] = { 0x28, 0xF7, 0x2C, 0x07, 0xD6, 0x01, 0x3C, 0xA7  };

OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

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
  sensors.begin();
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
    getSensorValues();
  }
}

void getSensorValues() {
  sensors.requestTemperatures(); 
  tempSensor1 = sensors.getTempC(sensor1);
  Serial.print(tempSensor1);
  Serial.println(" ºC"); 
  client.publish(outTopic, String(tempSensor1).c_str(), true);
  delay(200);
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

  if (String(topic) == inTopic) {
    
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
