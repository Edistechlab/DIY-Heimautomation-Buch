/*
Project:  Edis Techlab Garduino Basic with MQTT
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 22.05.2021
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
#define ESPHostname "GarduinoBASIC"
String clientId = "GarduinoBASIC-"; 

#define soilMoist_topic "Bodenfeuchte" 
#define soilMoistPercent_topic "BodenfeuchteProzent" 
#define pumpStatus_topic "Pumpe" 
#define inTopic "GarduinoBASIC/intopic"
#define outTopic "GarduinoBASIC/outTopic"

const int sensorTaktSek = 60;         // alle x Sekunden wird der Sensor ausgelesen
const int soilMoistLevelLow = 570;    // Wert ohne Kontakt zum Boden (trocken)
const int soilMoistLevelHigh = 192;   // Wert im Wasser
const int startWatering = 20;         // Schwellwert Bewässerung ein in %
const int wateringDuration = 7;       // Bewässerungszeit in Sekunden
const int maxWateringDay = 2;         // Wie oft pro Tag bewässert werden soll (empfohlen max. 2x)
const int soilMoistPin = A0;          // ADC Pin
const int pumpPin = 5;                // D1

int lastSoilMoist = 0;
int waterPerDayCount = 0;
int sensorTakt = sensorTaktSek * 1000;   // Umrechnung von ms auf Sekunden
long lastMsg = 0;
long todayTimer = 24 * 3600 * 1000 / maxWateringDay;  // Umrechnung von einem Tag auf ms / 24 h * 60 Min * 60 Sek * 1000 durch Anz. Bewässerung
long lastDay = 0;
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
  
  if (now - lastDay > todayTimer) {   
    lastDay = now;
    Serial.println("Tagesbewässerungsanzahl zurück gesetzt!");
    waterPerDayCount = 0;
  }
}

void getSoilMoistValues() {
  int newSoilMoist = analogRead(soilMoistPin);

  if (newSoilMoist != lastSoilMoist) {
    Serial.print("Analog Value: ");
    Serial.print(newSoilMoist);
    client.publish(soilMoist_topic, String(newSoilMoist).c_str(), true);
    
    // Auswertung der Bodenfeuchtigkeit in Prozent
    int percentSoilMoist = map(newSoilMoist, soilMoistLevelHigh, soilMoistLevelLow, 100, 0);
    Serial.print("\t"); 
    Serial.print(percentSoilMoist);
    Serial.println(" %");
    client.publish(soilMoistPercent_topic, String(percentSoilMoist).c_str(), true);
    if (percentSoilMoist <= startWatering) {
      if (waterPerDayCount < maxWateringDay) {
        Serial.println("Boden zu trocken, Bewässerung aktivieren");
        plantWatering();
      }
    }
    lastSoilMoist = newSoilMoist;
  }
}

void plantWatering() {
  digitalWrite(pumpPin, HIGH); // Schaltet die Wasserpumpe ein
  Serial.println("Bewässerung START");
  client.publish(pumpStatus_topic, "ON");
  delay(wateringDuration*1000);
  
  digitalWrite(pumpPin, LOW); // Schaltet die Wasserpumpe aus
  Serial.println("Bewässerung STOPP");
  client.publish(pumpStatus_topic, "OFF");
  ++waterPerDayCount;
  Serial.print("Bewässerung heute: ");
  Serial.println(waterPerDayCount);
  delay(100);
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
