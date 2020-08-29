/*
Project:  DHT11 Temperatur und Luftfeuchte Sensor über MQTT und OTA update
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 29.08.2020
Version:  V1.0
IDE:      Arduino IDE 1.8.13
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V2.7.4 
 - Board: esp32   by Espressif Systems   V1.0.2

Required libraries (sketch -> include library -> manage libraries)
 - Adafruit_DHT sensor library V1.3.10 
 - Adafruit Unified Sensor Library V1.1.4
 - PubSubClient by Nick ‘O Leary V2.0.8
 - ArduinoOTA by Juraj Andrassy V1.0.5
 
Wirering for the DHT11 Sensor:
DHT11       ESP32       ESP8266     UNO         Mega
VCC         3.3V        3.3V        3.3V        3.3V      
GND         GND         GND         GND         GND
OUT         22          D1/5        A5          21
*/

#include <DHT.h>
#include <DHT_U.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include <Adafruit_Sensor.h>

#define wifi_ssid "Your_SSID"
#define wifi_password "Your_Password"
#define mqtt_server "MQTT_Server_IP"
#define mqtt_user "MQTT_username"         
#define mqtt_password "MQTT_PW"
#define ESPHostname "DHT11_Sensor"

#define temperature_topic "esp01/temperature"
#define humidity_topic "esp01/humidity"
#define inTopic "esp01/inTopic"
#define outTopic "esp01/outTopic"

#define DHTPIN 5        //Pin an dem das OUT Signal angeschlossen ist.
float temp = 0.00;
float hum = 0.00;
float tempDiff = 0.2;
float humDiff = 1.0;
int sensorTakt = 2000; //alle 2 Sekunden wird der Sensor ausgelesen
DHT_Unified dht(DHTPIN, DHT11);  

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
    getDHT11Values();
  }
} 

void getDHT11Values() {
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    float newTemp = event.temperature;
    Serial.println( String(newTemp));
    dht.humidity().getEvent(&event);
    float newHum = event.relative_humidity;
    Serial.println( String(newHum));
    
    if (checkBound(newTemp, temp, tempDiff)) {
      temp = newTemp;
      Serial.print("Temperature:");
      Serial.println(String(temp).c_str());
      client.publish(temperature_topic, String(temp).c_str(), true);
    }

    if (checkBound(newHum, hum, humDiff)) {
      hum = newHum;
      Serial.print("Huminity:");
      Serial.println(String(hum).c_str());
      client.publish(humidity_topic, String(hum).c_str(), true);
    }
    delay(1000);
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
