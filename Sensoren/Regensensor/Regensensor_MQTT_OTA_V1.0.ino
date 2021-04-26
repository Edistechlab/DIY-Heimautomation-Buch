/*
Project:  Raindrop Sensor - MH-RD / YL-83 / FC-37 with MQTT und OTA update
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 26.04.2021
Version:  V1.0
IDE:      Arduino IDE 1.8.13
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V2.7.4 
 - Board: esp32   by Espressif Systems   V1.0.4

Required libraries (sketch -> include library -> manage libraries)
 - PubSubClient by Nick ‘O Leary V2.8.0
 - ArduinoOTA by Juraj Andrassy V1.0.5

Wirering for the Module:
Modul       ESP32       ESP8266
VCC         D4/4        D2/4
GND         GND         GND
DO          D5/5        D1/5
AO          VP/A0/36    A0
*/

#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include <WiFi.h>  // ESP32 only

#define wifi_ssid "Your_SSID"
#define wifi_password "Your_Password"
#define mqtt_server "MQTT_Server_IP"
#define mqtt_user "MQTT_username"         
#define mqtt_password "MQTT_PW"
#define ESPHostname "Regensensor"
String clientId = "Regensensor-"; 

#define regenStatus_topic "RegenStatus" 
#define regenAnalog_topic "RegenAnalog" 
#define regenDigital_topic "RegenDigital" 
#define inTopic "Regenensor/intopic"
#define outTopic "Regenensor/outTopic"

const int rainDigitalPin = 5;
const int rainAnalogPin = A0;
const int rainSensorPowerPin = 4;
const int rainLight = 3500;    //Bei ESP8266 - 980 als Richtwert
const int rainHeavy = 3000;    //Bei ESP8266 - 900 als Richtwert
const int sensorTakt = 2000; //alle 2 Sekunden wird der Sensor ausgelesen

int lastRainDigital = 0;
int rainAnalog = 4095;
long lastMsg = 0;
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
    
  pinMode(rainDigitalPin, INPUT);
  pinMode(rainAnalogPin, INPUT);
  pinMode(rainSensorPowerPin, OUTPUT);
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
    getRainValues();
  }
}

void getRainValues() {
  digitalWrite(rainSensorPowerPin, HIGH); // Schaltet den Strom für den Sensor ein
  delay(100);
  int newRainDigital = digitalRead(rainDigitalPin);
  int newRainAnalog = analogRead(rainAnalogPin);
  
  if (newRainDigital != lastRainDigital) {
    if (newRainDigital == HIGH) {
      client.publish(regenDigital_topic, "ON"); 
      Serial.print("Digital Info: Kein Regen\n");
    } else {
      client.publish(regenDigital_topic, "OFF"); 
      Serial.print("Digital Info: Regen\n");
    }
    // Kurze Pause zum entprellen
    delay(50);
  }
  lastRainDigital = newRainDigital;

   rainAnalog = newRainAnalog;
      Serial.print("RegenAnalog: ");
      Serial.println(rainAnalog);
      client.publish(regenAnalog_topic, String(rainAnalog).c_str(), true);
      if (rainAnalog <= rainLight) {
        if (rainAnalog <= rainHeavy) {
          Serial.print("Starker Regen\n");
          client.publish(regenStatus_topic, "Starker Regen");
        }
        else {
          Serial.print("Leichter Regen\n");
          client.publish(regenStatus_topic, "Leichter Regen");
        }
      }
      else {
        Serial.print("Kein Regen\n");
        client.publish(regenStatus_topic, "Kein Regen");
      }
      
  digitalWrite(rainSensorPowerPin, LOW);  // Schaltet den Strom für den Sensor aus
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
    //Auswertung wenn Daten die in den ESP kommen
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
