 /*
Project:  Water Tank 
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 14.04.2024
Update:   
Version:  V1.0
IDE:      Arduino IDE 2.3.2

Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V3.1.2
 - Board: esp32 by Espressif Systems     V2.0.11 

Required libraries (Tools -> manage libraries)
 - PubSubClient by Nick ‘O Leary V2.8.0
 - ArduinoOTA by Juraj Andrassy V1.1.0
 - OneWire libary V2.3.7 by Jim Studt
 - DallasTemperature V3.9.0 by Miles Burton
 - NewPing libary V1.9.7 by Tim Eckel
 
Wemos D1 mini - NOTE: Relay has invertet signals
*/


#include "secrets.h"
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "NewPing.h"

#define ESPHostname "Water_Tank"
String clientId = "WaterTank-";

#define outTopic "waterTankOut"
#define waterValve1_Topic "waterValve1" // Fill tank with City water NC
#define waterValve1Status_Topic "waterValve1Status"
#define waterValve2_Topic "waterValve2" // City water NO
#define waterValve2Status_Topic "waterValve2Status"
#define waterValve3_Topic "waterValve3" // Tank water NC
#define waterValve3Status_Topic "waterValve3Status"
#define tankTemp_Topic "tankTemp" // Tank Temperature
#define fillLevel_Topic "tankFillLevel" // Tank Filling level
#define waterAmountLiter_Topic "waterAmountLiter" // Tank Filling level in Liter
#define waterAmountPercent_Topic "waterAmountPercent" // Tank Filling level in Liter

WiFiClient espClient;  
PubSubClient client(espClient);  

const uint8_t trigPin = 15;    //D8 - weiss
const uint8_t echoPin = 13;   //D7 - braun
const uint8_t oneWireBus = 12;  //D6 - gelb
const uint8_t valve1Pin = 5;    //D1 - blau ch4
const uint8_t valve2Pin = 4;    //D2 - weiss ch3
const uint8_t valve3Pin = 2;    //D4 - lila ch2

char msg[50];
int value = 0;
long lastMsg = 0;
int sensorTakt = 10000; //alle 10 Sekunden wird der Sensor ausgelesen
const uint8_t max_Distance = 400;  //400cm - 4m
const uint8_t tankCircumference = 3545;  // in mm
const int tankHight = 2000; // in mm
const int tankLiter = 2000; // in Liter

float distance;
byte startup = 0;

OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

NewPing sonar(trigPin, echoPin, max_Distance);

void setup() {
  Serial.begin(115200);
  setup_wifi();
  ArduinoOTA.setHostname("WaterTank_WemosD1mini");
  ArduinoOTA.setPassword("DrEd3006");
  ArduinoOTA.begin();
  sensors.begin();
   
  client.setServer(mqtt_broker, mqtt_port); 
  client.setCallback(callback); 

  pinMode(valve1Pin, OUTPUT);
  pinMode(valve2Pin, OUTPUT);
  pinMode(valve3Pin, OUTPUT);

  digitalWrite(valve1Pin, LOW); 
  digitalWrite(valve2Pin, LOW); 
  digitalWrite(valve2Pin, LOW); 
}

void loop() {
  if (!client.connected()) {  
    reconnect();  
  }
  client.loop();
  ArduinoOTA.handle();

  if (startup == 0) {
    startupSequence();
  }

  long now = millis();
  if (now - lastMsg > sensorTakt) {   
    lastMsg = now;
    getTemp();
    getDistance();
  }
}

void startupSequence() {
  digitalWrite(valve1Pin, LOW);
  client.publish(waterValve1Status_Topic, "state_closed");
  digitalWrite(valve2Pin, LOW);
  client.publish(waterValve2Status_Topic, "state_closed");
  digitalWrite(valve2Pin, LOW);
  client.publish(waterValve2Status_Topic, "state_closed");
  startup = 1;
}

void getTemp() {
  sensors.requestTemperatures(); 
  delay(200);
  float temperatureC = sensors.getTempCByIndex(0);
  Serial.print("Temp: ");
  Serial.println(temperatureC);
  client.publish(tankTemp_Topic, String(temperatureC).c_str(), true);
  delay(200);
}

void getDistance() {
  distance = sonar.ping_cm();
  int emptyVolume = distance * 10; //je cm 10 Liter Wasser
  int volumeWater = tankLiter - emptyVolume;
  int volumePercent = (volumeWater * 100) / tankLiter; 
  client.publish(fillLevel_Topic, String(distance).c_str(), true);
  client.publish(waterAmountLiter_Topic, String(volumeWater).c_str(), true);
  client.publish(waterAmountPercent_Topic, String(volumePercent).c_str(), true);
  delay(200);
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

  if (String(topic) == waterValve1_Topic) {   
    switchWaterValve1(messageTemp);
  }
  if (String(topic) == waterValve2_Topic) {   
    switchWaterValve2(messageTemp);
  }
  if (String(topic) == waterValve3_Topic) {   
    switchWaterValve3(messageTemp);
  }
}

void switchWaterValve1(String valve1) {
  if (valve1 == "OPEN") {
    digitalWrite(valve1Pin, HIGH); 
    client.publish(waterValve1Status_Topic, "state_open");
  }
  if (valve1 == "CLOSE") {
    digitalWrite(valve1Pin, LOW); 
    client.publish(waterValve1Status_Topic, "state_closed");
  }
}

void switchWaterValve2(String valve2) {
  if (valve2 == "OPEN") {
    digitalWrite(valve2Pin, HIGH);
    client.publish(waterValve2Status_Topic, "state_open");
  }
  if (valve2 == "CLOSE") {
    digitalWrite(valve2Pin, LOW); 
    client.publish(waterValve2Status_Topic, "state_closed");
  }
}

void switchWaterValve3(String valve3) {
  if (valve3 == "OPEN") {
    digitalWrite(valve3Pin, HIGH); 
    client.publish(waterValve3Status_Topic, "state_open");
  }
  if (valve3 == "CLOSE") {
    digitalWrite(valve3Pin, LOW); 
    client.publish(waterValve3Status_Topic, "state_closed");
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
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
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
      client.subscribe(waterValve1_Topic);
      client.subscribe(waterValve2_Topic);
      client.subscribe(waterValve3_Topic);
    } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        delay(5000);
      }
   }
}

