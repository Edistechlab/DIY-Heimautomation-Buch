/*
Project:  BME680 Sensor mit der BSEC Library, MQTT und OTA als JSON gesendet
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 01.03.2022
Version:  V1.0
IDE:      Arduino IDE 1.8.16
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V3.0.2 

Required libraries (sketch -> include library -> manage libraries)
 - BSEC Software Library by Bosch Sensortec Enviroment Cluster V1.6.1480
 - PubSubClient by Nick ‘O Leary V2.8.0
 - ArduinoOTA by Juraj Andrassy V1.0.7
 - ArduinoJson Library by Benolt Blanchon V6.19.2
*/

#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include "bsec.h"
Bsec bme680;

#define wifi_ssid "Your_SSID"
#define wifi_password "Your_Password"
#define mqtt_server "MQTT_Server_IP"
#define mqtt_user "MQTT_username"         
#define mqtt_password "MQTT_PW"
#define ESPHostname "BME680_01"
String clientId = "BME680_01-"; 

#define daten_topic "BME680_01/daten"
#define status_topic "BME680_01"
#define error_topic "BME680_01/error"
#define intopic "BME680_01-"

float temp = 1.00;
float hum = 1.00;
int druck = 1;
int IAQ = 10;
int IAQac = 0;
int eCO2 = 1;
int voc = 1;
int error_Status = 0;
const String error_Message="BME680 Sensor not found :-(";
const int sensorTakt = 2000; //alle Sekunden wird der Sensor ausgelesen
long lastMsg = 0;
char msg[50];
StaticJsonDocument<200> doc;
String sensorDatenJson; 

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
  
  Wire.begin(4,5);
  bme680.begin(BME680_I2C_ADDR_PRIMARY, Wire);
  bsec_virtual_sensor_t sensorList[7] = {
    BSEC_OUTPUT_IAQ,
    BSEC_OUTPUT_RAW_PRESSURE,
    BSEC_OUTPUT_STATIC_IAQ,
    BSEC_OUTPUT_CO2_EQUIVALENT,
    BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
  };
  bme680.updateSubscription(sensorList, 7, BSEC_SAMPLE_RATE_LP);
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
    getBME680Values();
  }
}

void getBME680Values() {
  if (bme680.run()) { 
      error_Status = 0;
      client.publish(error_topic, String(error_Status).c_str(), true);

      sensorDatenJson = "";
      temp = (bme680.temperature);
      Serial.print("Temp: "); Serial.print(String(temp).c_str()); Serial.print(" C\t");
      doc["Temperatur"] = temp;  
      hum = (bme680.humidity);
      Serial.print("Hum: "); Serial.print(String(hum).c_str()); Serial.print(" %\t");
      doc["Luftfeuchtigkeit"] = hum;
      druck = (bme680.pressure)/100;
      Serial.print("Druck: "); Serial.print(String(druck).c_str()); Serial.print(" hPa\t");
      doc["Luftdruck"] = druck;
      IAQ = (bme680.iaq);
      Serial.print("IAQ: "); Serial.print(String(IAQ).c_str()); Serial.print("\t");
      doc["IAQ"] = IAQ;
      IAQac = (bme680.iaqAccuracy);
      Serial.print("IAQ-accuracy: "); Serial.print(String(IAQac).c_str()); Serial.print("\t");
      doc["iaq_Genauigkeit"] = IAQac;
      eCO2 = (bme680.co2Equivalent);
      Serial.print("CO2: "); Serial.print(String(eCO2).c_str()); Serial.print("\t");
      doc["eCO2"] = eCO2;
      voc = (bme680.breathVocEquivalent);
      Serial.print("VOC: "); Serial.print(String(voc).c_str()); Serial.println("\t");
      doc["VOC"] = voc;
      serializeJson(doc, sensorDatenJson);
      client.publish(daten_topic, String(sensorDatenJson).c_str(), true);
    } 
    else if (bme680.bme680Status != BME680_OK) {
        error_Status = 1;
        Serial.println(String(error_Message).c_str());
        client.publish(status_topic, String(error_Message).c_str(), true);
        client.publish(error_topic, String(error_Status).c_str(), true);        
        delay(1000);
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
  if (String(topic) == intopic) {
    // Room for Code on an intopic
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
      client.publish(status_topic, ESPHostname);
      // ... and resubscribe
      client.subscribe(intopic);
  } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
