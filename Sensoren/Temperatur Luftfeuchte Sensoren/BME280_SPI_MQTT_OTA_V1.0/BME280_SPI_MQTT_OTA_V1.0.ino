/*
Project:  BME280 I2C Temperatur und Luftfeuchte Sensor über MQTT und OTA update
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 20.09.2020
Version:  V1.0
IDE:      Arduino IDE 1.8.13
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V2.7.4 
 - Board: esp32   by Espressif Systems   V1.0.2

Required libraries (sketch -> include library -> manage libraries)
 - Adafruit BME280 library V2.1.0 
 - PubSubClient by Nick ‘O Leary V2.8.0
 - ArduinoOTA by Juraj Andrassy V1.0.5
 
Wirering for the BME280 Sensor:
BME280      ESP32       ESP8266     UNO         Mega
VCC         3.3V        3.3V        3.3V        3.3V      
GND         GND         GND         GND         GND
SCL         D30/18      D5/14       13          52
SDA         D37/23      D7/13       11          50
CSB         D29/5       D8/15       10          53
SDO         D31/19      D6/12       12          51
*/

#include <Adafruit_BME280.h>
//NodeMCU ESP8266
#define BME_SCK 14   // Serial Clock 
#define BME_MOSI 13  // Serial Data In 
#define BME_CS 15     // Chip Select 
#define BME_MISO 12  // Serial Data Out  
Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI
#include <PubSubClient.h>
#include <ArduinoOTA.h>

#define wifi_ssid "Your_SSID"
#define wifi_password "Your_Password"
#define mqtt_server "MQTT_Server_IP"
#define mqtt_user "MQTT_username"         
#define mqtt_password "MQTT_PW"
#define ESPHostname "BME280_Sensor"

#define temperature_topic "esp01/temperature"
#define humidity_topic "esp01/humidity"
#define pressure_topic "esp01/pressure"
#define inTopic "esp01/inTopic"
#define outTopic "esp01/outTopic"

float temp = 0.00;
float hum = 0.00;
float pressure = 0.00;
float tempDiff = 0.2;
float humDiff = 1.0;
float pressureDiff = 0.2;
int sensorTakt = 5000; //alle 5 Sekunden wird der Sensor ausgelesen

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
  if (!bme.begin(0x76, &Wire)) {     //I2C Adresse ist 0x76 oder 0x77
    Serial.println("BME280 Sensor nicht gefunden, überprüfe die Verkabelung oder Adresse!");
    while (true);
  }
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
    getBME280Values();
  }
} 

void getBME280Values() {
    float newTemp = bme.readTemperature();
    float newHum = bme.readHumidity();
    float newPressure = bme.readPressure()/100;  //von Pa auf hPa umrechnen
    
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

    if (checkBound(newPressure, pressure, pressureDiff)) {
      pressure = newPressure;
      Serial.print("Pressure:");
      Serial.println(String(pressure).c_str());
      client.publish(pressure_topic, String(pressure).c_str(), true);
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
