 /*
Project:  MQTT Test (LED and Button) with ESP32 / NodeMCU and OTA update
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 17.01.2021 Update: 25.01.2024
Version:  V1.1
IDE:      Arduino IDE 2.1.1

Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V3.1.2 
 - Board: esp32   by Espressif Systems   V2.0.11

Required libraries (Tools -> manage libraries)
 - PubSubClient by Nick ‘O Leary V2.8
 - ArduinoOTA by Juraj Andrassy V1.1.0

Wirering Pins:
LED     GPIO22
Button  GPIO23
*/ 

#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include <WiFi.h>  // ESP32 only

// Wi-Fi Settings
const char *wifi_ssid = "Your WiFi SSID";
const char *wifi_password = "Your WiFi Password";

// MQTT Settings
const char *mqtt_broker = "MQTT Broker IP";
const char *mqtt_user = "MQTT Username";
const char *mqtt_password = "MQTT Password";
const char *ESPHostname = "ESP32_MQTT_Test";
const int mqtt_port = 1883;
String client_id = "ESP32-";

// define Topics 
const char *ledTopicEsp32 = "esp32/ledState";
const char *btnTopicEsp32 = "esp32/btnState";
const char *btnTopicEsp8266 = "esp8266/btnState";
   
WiFiClient espClient;  
PubSubClient client(espClient);  

const int LEDPin = 22;
const int buttonPin = 23;
byte buttonState = LOW;
byte lastButtonState = LOW;
    
void setup() {  
  Serial.begin(115200);
  setup_wifi();
  ArduinoOTA.setHostname(ESPHostname);
  ArduinoOTA.setPassword("admin");
  ArduinoOTA.begin();
   
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);  
  pinMode(LEDPin, OUTPUT);
  pinMode(buttonPin, INPUT);
}  
   
void loop() {  
  if (!client.connected()) {  
    reconnect();  
  }
  client.loop();
  ArduinoOTA.handle();
  
  buttonState = digitalRead(buttonPin);
  if (buttonState != lastButtonState) {
    if (buttonState == LOW) {
      client.publish(btnTopicEsp32, "OFF"); 
      Serial.print("Changing Button to OFF\n");
    } else {
      client.publish(btnTopicEsp32, "ON");
      Serial.print("Changing Button to ON\n");
    }
    delay(100);
  }
  lastButtonState = buttonState;
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
  Serial.print(" Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  if (String(topic) == btnTopicEsp8266) {
    if(messageTemp == "ON"){
      Serial.print("Changing LED to ON\n");
      digitalWrite(LEDPin, HIGH);
      client.publish(ledTopicEsp32, "ON");
      delay(200);
    }
    else if (messageTemp == "OFF") {
      Serial.print("Changing LED to OFF\n");
      digitalWrite(LEDPin, LOW);
      client.publish(ledTopicEsp32, "OFF"); 
      delay(200);
    }
  }
}
      
 void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    client_id += String(WiFi.macAddress());
    // Attempt to connect
    if (client.connect(client_id.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("esp32/outtopic", ESPHostname);
      // ... and resubscribe
      client.subscribe(btnTopicEsp8266);
    } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        delay(5000);
      }
   }
}
