 /*
Project:  Relay switch over MQTT with ESP8266 / NodeMCU and OTA update
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 23.10.2020 
Version:  V1.0
IDE:      Arduino IDE 1.8.13

Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V2.7.4 

Required libraries (Tools -> manage libraries)
 - PubSubClient by Nick ‘O Leary V2.8.0
 - ArduinoOTA by Juraj Andrassy V1.0.5

Wirering for the Output:
Relay      NodeMCU
VCC         3.3V
GND         G
IN1         D5 / GPIO14
*/

#include <PubSubClient.h>
#include <ArduinoOTA.h>

#define wifi_ssid "Your_SSID"
#define wifi_password "Your_Password"
#define mqtt_server "MQTT_Server_IP"
#define mqtt_user "MQTT_username"         
#define mqtt_password "MQTT_PW"
#define ESPHostname "MQTT_Relay"     

#define state_topic "MQTT_Relay/state" 
#define inTopic "MQTT_Relay/command"
   
WiFiClient espClient;  
PubSubClient client(espClient);  

const int OutputPin = 14;  // der Output Pin wo das Relay angehängt ist
    
void setup() {  
  Serial.begin(115200);
  setup_wifi();
  ArduinoOTA.setHostname(ESPHostname);
  // ArduinoOTA.setPassword("admin");
  ArduinoOTA.begin();
   
  client.setServer(mqtt_server, 1883); 
  client.setCallback(callback);  
  pinMode(OutputPin, OUTPUT);
  digitalWrite(OutputPin, HIGH);  // Relay OFF - Invertiertes Signal deswegen HIGH
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

  if (String(topic) == inTopic) {
    if(messageTemp == "ON"){
      Serial.print("Changing output to ON\n");
      digitalWrite(OutputPin, LOW);   //Invertiertes Signal
      client.publish(state_topic, "ON");
      delay(200);
    }
    else if(messageTemp == "OFF"){
      Serial.print("Changing output to OFF\n");
      digitalWrite(OutputPin, HIGH);  //Invertiertes Signal
      client.publish(state_topic, "OFF"); 
      delay(200);
    }
  }
}
      
 void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "MQTT_Relay-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(state_topic, "MQTT_Relay alive");
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
