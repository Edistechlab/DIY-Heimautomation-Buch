/*
Project:  BME280 Sensor data send over MQTT with a ESP8266 / NodeMCU with Deep-Sleep
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 10.04.2020
Version:  V1.0
 
Required libraries (Tools -> manage libraries)
 - PubSubClient by Nick O'Leary V2.7.0  
 - Adafruit BME280 Library V2.0.1
 - Adafruit Unified Sensor V1.1.2
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community V2.6.3

Wirering:
Bridge RST with GPIO16 (D0) - After the sketch is uploaded
BME280      NodeMCU
VCC         3.3V
GND         G
SCL         D1 / GPIO5
SDA         D2 / GPIO4
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h> 
   
#define wifi_ssid "Your_SSID"
#define wifi_password "Your_Password"

#define mqtt_server "MQTT_Server_IP"
#define mqtt_user "MQTT_username"         
#define mqtt_password "MQTT_PW"     

#define humidity_topic "esp01/humidity"
#define temperature_topic "esp01/temperature"
#define pressure_topic "esp01/pressure"
#define status_topic "esp01/status" 
    
#define durationSleep  10   // Sekunden  
   
Adafruit_BME280 bme; // I2C  
WiFiClient espClient;  
PubSubClient client(espClient);  
   
float temp = 0.0;  
float hum = 0.0;  
float pressure = 0.0;  
bool status;  
    
 void setup() {  
  Serial.begin(115200);   
  status = bme.begin(0x76, &Wire);   //I2C address is either 0x76 or 0x77
  if (!status) {  
     Serial.println("BME280 Sensor nicht gefunden, überprüfe die Verkabelung, Adresse, oder die Sensor ID!");  
     while (1);  
  }  
  setup_wifi();  
  client.setServer(mqtt_server, 1883);  
  if (!client.connected()) {  
    reconnect();  
  }  
  client.loop();  
    
  temp=bme.readTemperature();   
  hum=bme.readHumidity();  
  pressure=(bme.readPressure()/100.0F); 
     
  Serial.print(F("Temperature = ")); 
  Serial.print( String(temp).c_str()); 
  Serial.println("C");
  client.publish(temperature_topic, String(temp).c_str(), true); 
   
  Serial.print(F("Pressure = ")); 
  Serial.print( String(pressure).c_str()); 
  Serial.println("hPa");
  client.publish(pressure_topic, String(pressure).c_str(), true);  
  
  Serial.print(F("Humidity = ")); 
  Serial.print(String(hum).c_str()); 
  Serial.println("%"); 
  client.publish(humidity_topic, String(hum).c_str(), true);  
  
  client.publish(status_topic, "ESP01 deep-sleep");
  delay(1000);  //Zeit zum übertrage von den MQTT Daten
  Serial.println("ESP01 geht in deep sleep");  
  ESP.deepSleep(durationSleep * 1e6);  
 }  
   
 void loop() {  
 }  
   
 void setup_wifi() {  
  delay(10);  
  // We start by connecting to a WiFi network  
  Serial.println();  
  Serial.print("Connecting to ");  
  Serial.println(wifi_ssid);  
  int _try = 0;
  WiFi.begin(wifi_ssid, wifi_password);  
  while (WiFi.status() != WL_CONNECTED) {  
   Serial.print("."); 
   delay(500);  
   _try++;      // Wenn nach 10 Versuchen nicht mit WiFi verbunden werden kann, deep-sleep
   if ( _try >= 10 ) {
        Serial.println("Kann nicht mit WiFi verbunden werden, gehe in deep-sleep");
        ESP.deepSleep(durationSleep * 1e6);
    } 
  }  
  Serial.println("");  
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");  
  Serial.println(WiFi.localIP());  
 }  
   
 void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(status_topic, "ESP01 alive");
    } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        delay(5000);
      }
   }
}
