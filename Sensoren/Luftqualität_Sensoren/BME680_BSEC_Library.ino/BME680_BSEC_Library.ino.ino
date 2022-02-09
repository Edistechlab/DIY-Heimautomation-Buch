/*
Project:  BME680 Sensor mit der BSEC Library
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 08.02.2022
Version:  V1.0
IDE:      Arduino IDE 1.8.16
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community   V3.0.2 

Required libraries (sketch -> include library -> manage libraries)
 - BSEC Software Library by Bosch Sensortec Enviroment Cluster V1.6.1480
*/

#include "bsec.h"
Bsec bme680;

void setup() {
  Serial.begin(115200);
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
  if (bme680.run()) { 
    Serial.print("Temp: "); Serial.print(bme680.temperature); Serial.print(" C\t");
    Serial.print("Hum: "); Serial.print(bme680.humidity); Serial.print(" %\t");
    Serial.print("Druck: "); Serial.print(bme680.pressure); Serial.print(" Pa\t");
    Serial.print("IAQ: "); Serial.print(bme680.iaq); Serial.print("\t");
    Serial.print("IAQ-Ac: "); Serial.print(bme680.iaqAccuracy); Serial.print("\t");
    Serial.print("CO2: "); Serial.print(bme680.co2Equivalent); Serial.print("\t");
    Serial.print("VOC: "); Serial.println(bme680.breathVocEquivalent);
  }
  else if (bme680.bme680Status != BME680_OK) {
      Serial.println("Sensor not found :-(");
      delay(1000);
  }
}
