/*
Project:  Edis Techlab Garten-Wemos Pro with MQTT controlled from a Server
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 21.08.2021
Version:  V1.0
IDE:      Arduino IDE 1.8.15
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V3.0.2  

Required libraries (sketch -> include library -> manage libraries)
 - PubSubClient by Nick ‘O Leary V2.8.0
 - ArduinoOTA by Juraj Andrassy V1.0.7
 - Adafruit SSD1306 V2.4.6
 - Adafruit GFX Library V1.10.10
*/

#include <PubSubClient.h>
#include <ArduinoOTA.h>

#define wifi_ssid "Your_SSID"
#define wifi_password "Your_Password"
#define mqtt_server "MQTT_Server_IP"
#define mqtt_user "MQTT_username"         
#define mqtt_password "MQTT_PW"
#define ESPHostname "GartenWemosPro"
String clientId = "GartenWemosPro-"; 

// Topics definieren
#define soilMoist1_topic "Bodenfeuchte1"
#define soilMoist2_topic "Bodenfeuchte2" 
#define soilMoist3_topic "Bodenfeuchte3" 
#define LDR_topic "LDR"
#define waterLevel_topic "Wasserstand" 
#define analogIn_topic "AnalogSensor"
#define btn_topic "Taster"
#define out1_topic "Out1" 
#define out2_topic "Out2"
#define out1Status_topic "StatusOut1" 
#define out2Status_topic "StatusOut2"
#define led_topic "LED" 
#define outTopic "GartenWemosPro/outTopic"

// Pins zuweisen
const int analogInPin = A0;     // ADC Pin
const int out1Pin = 12;         // D6
const int out2Pin = 13;         // D7
const int ledRot = 1;           // Tx
const int ledGruen = 3;         // Rx
const int manButton = 2;        // D4
const int mulitplexA0 = 0;      // D3
const int mulitplexA1 = 16;     // D0
const int mulitplexA2 = 14;     // D5
const int waterLevel = 15;      // D8

// Diverse Variablen
const int sensorTaktSek = 60;   // alle x Sekunden werden die Sensoren ausgelesen
int btnState = LOW;
int waterState = HIGH;
int errorState = LOW;
long lastMsg = 0;
char msg[50];
int messung[2];
const int sensorTakt = sensorTaktSek * 1000;   // Umrechnung von ms auf Sekunden

WiFiClient espClient;  
PubSubClient client(espClient);  

void setup() {
  Serial.begin(9600);
  setup_wifi();
  ArduinoOTA.setHostname(ESPHostname);
  // ArduinoOTA.setPassword("admin");
  ArduinoOTA.begin();
  client.setServer(mqtt_server, 1883); 
  client.setCallback(callback); 
  
  pinMode(analogInPin, INPUT);
  pinMode(manButton, INPUT);
  pinMode(waterLevel, INPUT);
  pinMode(out1Pin, OUTPUT);
  pinMode(out2Pin, OUTPUT);
  pinMode(ledRot, OUTPUT);
  pinMode(ledGruen, OUTPUT);
  pinMode(mulitplexA0, OUTPUT);
  pinMode(mulitplexA1, OUTPUT);
  pinMode(mulitplexA2, OUTPUT);

  digitalWrite(out1Pin, LOW); // Sicherstellen das der Ausgang aus ist
  digitalWrite(out2Pin, LOW); // Sicherstellen das der Ausgang aus ist

  delay(1000);
}

void loop() {
  if (!client.connected()) {reconnect();}
  client.loop();
  ArduinoOTA.handle(); 
  long now = millis();

  if (now - lastMsg > sensorTakt) {   
    lastMsg = now;
    getSensorValue("CH1");
    getSensorValue("CH2");
    getSensorValue("CH3");
    getSensorValue("CH4");
  }
  checkButton();
  checkWaterLevel();
}

void switchLED(String messageLED) {
  if (messageLED == "rot") {setLedRed();}
  if (messageLED == "gruen") {setLedGreen();}
  if (messageLED == "gelb") {setLedYellow();}
  if (messageLED == "OFF") {setLedOff();}
}

void checkButton() {
  if (digitalRead(manButton) == HIGH) {
    if (btnState == LOW) {
      client.publish(btn_topic, "HIGH");
      setOutput1("ON");   //Setzt den Ausgang 1 beim Tastendruck
      setOutput2("ON");   //Setzt den Ausgang 2 beim Tastendruck
      btnState = HIGH;
    }
  } 
  else {
    if (btnState == HIGH){
      client.publish(btn_topic, "LOW");
      btnState = LOW;
      setOutput1("OFF");  //Schaltet den Ausgang 1 ab
      setOutput2("OFF");  //Schaltet den Ausgang 2 ab
    }
  }
}

void checkWaterLevel() {
  if (digitalRead(waterLevel) == HIGH) {
    if (waterState == HIGH) {
      waterState = LOW;
      client.publish(waterLevel_topic, "LOW");
      setLedRed();
      errorState = HIGH;
    }
  }
  else {
    if (waterState == LOW){
      waterState = HIGH;
      setLedGreen();
      errorState = LOW;
    }
  }
}

void startupSequenz() {
  checkWaterLevel();
  if (errorState == LOW) {setLedGreen();}
}

void getSensorValue(String channel) {
    if (channel == "CH1") {
      digitalWrite(mulitplexA0, HIGH);
      digitalWrite(mulitplexA1, LOW);
      digitalWrite(mulitplexA2, LOW);
      delay(500);
      int Sensor1 = analogRead(analogInPin);
      client.publish(soilMoist1_topic, String(Sensor1).c_str(), true);
    }
    if (channel == "CH2") {
      digitalWrite(mulitplexA0, LOW);
      digitalWrite(mulitplexA1, HIGH);
      digitalWrite(mulitplexA2, LOW);
      delay(500);
      int Sensor2 = analogRead(analogInPin);
      client.publish(soilMoist2_topic, String(Sensor2).c_str(), true);
    }
    if (channel == "CH3") {
      digitalWrite(mulitplexA0, HIGH);
      digitalWrite(mulitplexA1, HIGH);
      digitalWrite(mulitplexA2, LOW);
      delay(500);
      int Sensor3 = analogRead(analogInPin);
      client.publish(soilMoist3_topic, String(Sensor3).c_str(), true);
    }
    if (channel == "CH4") {
      digitalWrite(mulitplexA0, LOW);
      digitalWrite(mulitplexA1, LOW);
      digitalWrite(mulitplexA2, HIGH);
      delay(500);
      int LDRValue = analogRead(analogInPin);
      client.publish(LDR_topic, String(LDRValue).c_str(), true); 
    }
}

void setLedRed() {
  digitalWrite(ledGruen, LOW);
  digitalWrite(ledRot, HIGH);
}

void setLedGreen() {
  digitalWrite(ledRot, LOW);
  digitalWrite(ledGruen, HIGH);
}

void setLedYellow() {
  digitalWrite(ledRot, HIGH);
  digitalWrite(ledGruen, HIGH);
}

void setLedOff() {
  digitalWrite(ledRot, LOW);
  digitalWrite(ledGruen, LOW);
}

void setOutput1(String Output1) {
  if (Output1 == "ON") {
    if (waterState == HIGH) {
      digitalWrite(out1Pin, HIGH); 
      client.publish(out1Status_topic, "ON");
      setLedYellow();
    }
    else {client.publish(out1Status_topic, "Kein Wasser!");}
  } 
  if (Output1 == "OFF") {
    digitalWrite(out1Pin, LOW); 
    client.publish(out1Status_topic, "OFF");
    if (errorState == LOW) {setLedGreen();}
  }
}

void setOutput2(String Output2) {
  if (Output2 == "ON") {
    if (waterState == HIGH) {
      digitalWrite(out2Pin, HIGH); 
      client.publish(out2Status_topic, "ON");
      setLedYellow();
    }
    else {client.publish(out1Status_topic, "Kein Wasser!");}
  }
  if (Output2 == "OFF") {
    digitalWrite(out2Pin, LOW); 
    client.publish(out2Status_topic, "OFF");
    if (errorState == LOW) {setLedGreen();}
  }
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
  String messageTemp;
  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }
  if (String(topic) == led_topic) {      
    switchLED(messageTemp);
  }
  if (String(topic) == analogIn_topic) {      
    getSensorValue(messageTemp);
  }
  if (String(topic) == out1_topic) {      
    setOutput1(messageTemp);
  }
  if (String(topic) == out2_topic) {      
    setOutput2(messageTemp);
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
      errorState == LOW;
      setLedGreen();
      startupSequenz();
      // Once connected, publish an announcement...
      client.publish(outTopic, ESPHostname);
      // ... and resubscribe
      client.subscribe(led_topic);
      client.subscribe(analogIn_topic);
      client.subscribe(out1_topic);
      client.subscribe(out2_topic);
    } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        setLedRed();
        errorState = HIGH;
        // Wait 5 seconds before retrying
        delay(5000);
      }
   }
}
