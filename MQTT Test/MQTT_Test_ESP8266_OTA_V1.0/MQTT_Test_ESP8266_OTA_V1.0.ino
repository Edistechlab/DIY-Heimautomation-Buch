 /*
Project:  MQTT Test (LED and Switch) with ESP8266 / NodeMCU and OTA update
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 17.01.2021 
Version:  V1.0
IDE:      Arduino IDE 1.8.13

Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V2.7.4 
 - Board: esp32   by Espressif Systems   V1.0.4

Required libraries (Tools -> manage libraries)
 - PubSubClient by Nick ‘O Leary V2.8.0
 - ArduinoOTA by Juraj Andrassy V1.0.5

Wirering for the Output:
LED     GPIO5 / D1
Switch  GPIO4 / D2
*/ 

#include <PubSubClient.h>
#include <ArduinoOTA.h>

#define wifi_ssid "Your_SSID"
#define wifi_password "Your_Password"
#define mqtt_server "MQTT_Server_IP"
#define mqtt_user "MQTT_username"         
#define mqtt_password "MQTT_PW"
#define ESPHostname "ESP8266_MQTT_Test" 
String clientId = "ESP8266-";      

#define outTopic "esp8266/outtopic" 
#define inTopic "esp8266/intopic"
   
WiFiClient espClient;  
PubSubClient client(espClient);  

const int LEDPin = 5;     // der Output Pin wo das LED angehängt ist
const int buttonPin = 4;  // der Input Pin wo der Taster angehängt ist
int buttonState = 0;
int lastButtonState = 0;
    
void setup() {  
  Serial.begin(115200);
  setup_wifi();
  ArduinoOTA.setHostname(ESPHostname);
  // ArduinoOTA.setPassword("admin");
  ArduinoOTA.begin();
   
  client.setServer(mqtt_server, 1883); 
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
    if (buttonState == HIGH) {
      client.publish("esp8266/outtopic", "OFF"); 
      Serial.print("Changing Button to OFF\n");
      client.publish("esp32/intopic", "OFF");
    } else {
      client.publish("esp8266/outtopic", "ON");
      Serial.print("Changing Button to ON\n");
      client.publish("esp32/intopic", "ON");
    }
    // Kurze Pause zum entprellen
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
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  if (String(topic) == inTopic) {
    if(messageTemp == "ON"){
      Serial.print("Changing LED to ON\n");
      digitalWrite(LEDPin, HIGH);   //Invertiertes Signal
      client.publish(outTopic, "ON");
      delay(200);
    }
    else if(messageTemp == "OFF"){
      Serial.print("Changing LED to OFF\n");
      digitalWrite(LEDPin, LOW);  //Invertiertes Signal
      client.publish(outTopic, "OFF"); 
      delay(200);
    }
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
