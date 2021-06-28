/*
Project:  Edis Techlab Garduino Basic with MQTT controlled from a Server
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 20.06.2021
Version:  V1.0
IDE:      Arduino IDE 1.8.13
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V2.7.4 

Required libraries (sketch -> include library -> manage libraries)
 - PubSubClient by Nick ‘O Leary V2.8.0
 - ArduinoOTA by Juraj Andrassy V1.0.5
*/

#include <PubSubClient.h>
#include <ArduinoOTA.h>

#define wifi_ssid "Your_SSID"
#define wifi_password "Your_Password"
#define mqtt_server "MQTT_Server_IP"
#define mqtt_user "MQTT_username"         
#define mqtt_password "MQTT_PW"
#define ESPHostname "GarduinoBASIC2"
String clientId = "GarduinoBASIC-"; 

#define soilMoist_topic "Bodenfeuchte2" 
#define pumpStatus_topic "PumpeStatus2" 
#define pump_topic "Pumpe2"
#define getSensorData_topic "getSensorData"
#define inTopic "GarduinoBASIC2/intopic"
#define outTopic "GarduinoBASIC2/outTopic"

const int sensorTaktSek = 60;         // alle x Sekunden wird der Sensor ausgelesen
const int soilMoistPin = A0;          // ADC Pin
const int pumpPin = 5;                // D1

int lastSoilMoist = 0;
int summeAnalog = 0;
int soilMoist[3];
int sensorTakt = sensorTaktSek * 1000;   // Umrechnung von ms auf Sekunden
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
    
  pinMode(soilMoistPin, INPUT);
  pinMode(pumpPin, OUTPUT);
  digitalWrite(pumpPin, LOW); // Sicherstellen das die Wasserpumpe aus ist
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
    getSoilMoistValues();
  }
  
}

void pump(String pumpOrder) {
  if (pumpOrder == "ON") {
    digitalWrite(pumpPin, HIGH); // Schaltet die Wasserpumpe ein
    Serial.println("Bewässerung START");
    client.publish(pumpStatus_topic, "ON");
  }
  
  if (pumpOrder == "OFF") {
    digitalWrite(pumpPin, LOW); // Schaltet die Wasserpumpe aus
    Serial.println("Bewässerung STOPP");
    client.publish(pumpStatus_topic, "OFF");
  }
}

void getSoilMoistValues() {
  for (int i = 0; i <= 2; i++) {
    soilMoist[i] = analogRead(soilMoistPin);
    Serial.print("Messwert ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(soilMoist[i]);
    summeAnalog = summeAnalog + soilMoist[i];
    delay(300);
  } 
  
  int mittelwertSoilMoist = summeAnalog / 3;
  Serial.print("Mittelwert: ");
  Serial.print(mittelwertSoilMoist);
  client.publish(soilMoist_topic, String(mittelwertSoilMoist).c_str(), true);
  mittelwertSoilMoist = 0;
  summeAnalog = 0;
}

void setup_wifi() {
  delay(10);
  // Verbindung zu Wi-Fi
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
  
  if (String(topic) == pump_topic) {      //Pumpe EIN oder AUS
    pump(messageTemp);
  }
  
  if (String(topic) == getSensorData_topic) {      
    getSoilMoistValues();
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
      client.subscribe(pump_topic);
      client.subscribe(getSensorData_topic);
    } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        delay(5000);
      }
   }
}
