/*
Project:  ESP32-CAM Time lapse - save pictures on SD-Card
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 07.11.2020
Version:  V1.0
IDE:      Arduino IDE 1.8.13
 
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp32   by Espressif Systems   V1.0.4
*/

#include "esp_camera.h"
#include "FS.h"
#include "SD_MMC.h"
#include "EEPROM.h"
#include "driver/rtc_io.h"
#include "soc/soc.h"           // Schaltet das brownour Problem aus
#include "soc/rtc_cntl_reg.h"  // Schaltet das brownour Problem aus

#define ID_ADDRESS            0x00
#define COUNT_ADDRESS         0x01
#define ID_BYTE               0xAA
#define EEPROM_SIZE           0x0F

#define sleepTime 10e6      // Deep Sleep Zeit 10 Sekunden
uint16_t fotoNummer = 0;

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);  // Schaltet den brownour Sensor aus
  Serial.begin(115200);
  pinMode(4, OUTPUT);
  rtc_gpio_hold_dis(GPIO_NUM_4); 
  
 // Pin Definition für CAMERA_MODEL_AI_THINKER aus dem Webserver Beispiel unter camera_pins.h
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0; 
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = 5;
  config.pin_d1 = 18;
  config.pin_d2 = 19;
  config.pin_d3 = 21;
  config.pin_d4 = 36;
  config.pin_d5 = 39;
  config.pin_d6 = 34;
  config.pin_d7 = 35;
  config.pin_xclk = 0;
  config.pin_pclk = 22;
  config.pin_vsync = 25;
  config.pin_href = 23;
  config.pin_sscb_sda = 26;
  config.pin_sscb_scl = 27;
  config.pin_pwdn = 32;
  config.pin_reset = -1;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA; // 1600x1200
    config.jpeg_quality = 10;           // Bild mit hoher Qualität
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // Kamera initialisieren
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Kamera Initialisierung fehlgeschlagen Fehler NR: 0x%x", err);
    return;
  }

  // Mounten der SD-Karte
  if(!SD_MMC.begin()) {
    Serial.println("Kann die SD-Karte nicht mounten");
    return;
  }
  
  uint8_t cardType = SD_MMC.cardType();
  if(cardType == CARD_NONE) {
    Serial.println("Keine SD-Karte eingelegt");
    return;
  }

  // EEPROM Initialisieren
  if (!EEPROM.begin(EEPROM_SIZE)) {
    Serial.println("Fehler beim EEPROM initialisieren"); 
    return;
  }
  
  if(EEPROM.read(ID_ADDRESS) != ID_BYTE) { 
    fotoNummer = 0;
    EEPROM.write(ID_ADDRESS, ID_BYTE);  
    EEPROM.commit(); 
  }
  else {
    EEPROM.get(COUNT_ADDRESS, fotoNummer);
    fotoNummer +=  1;    
  }

  // Foto machen
  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Fotografieren fehlgeschlagen!");
    return;
  }

  // Foto unter diesem Pfad speichern
  String path = "/Foto" + String(fotoNummer) + ".jpg";
  fs::FS &fs = SD_MMC;
  
  File file = fs.open(path.c_str(), FILE_WRITE);
  if(!file) {
    Serial.println("Fehler beim File erstellen"); 
  } 
  else {
    file.write(fb->buf, fb->len); 
    EEPROM.write(COUNT_ADDRESS, fotoNummer);
    EEPROM.commit();
  }
  
  file.close();
  esp_camera_fb_return(fb);
  
  Serial.printf("Foto gespeichert: %s\n", path.c_str());
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);            //flash LED aus
  rtc_gpio_hold_en(GPIO_NUM_4);
  delay(1000);
  Serial.println("Gehe in den deep-sleep mode");
  esp_sleep_enable_timer_wakeup(sleepTime);
  esp_deep_sleep_start();
}

void loop() {
}
