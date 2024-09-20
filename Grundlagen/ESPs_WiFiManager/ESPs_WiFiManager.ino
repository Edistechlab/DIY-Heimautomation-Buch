// Für ESP8266: #include <ESP8266WiFi.h>
// Für ESP32: #include <WiFi.h>
#include <WiFiManager.h> // WiFiManager Bibliothek

void setup() {
  // Serielle Schnittstelle für Debugging starten
  Serial.begin(115200);

  // WiFiManager-Instanz erstellen
  WiFiManager wm;
  wm.resetSettings();

  // Setze den ESP in den Access-Point-Modus, wenn keine Verbindung besteht
  if (!wm.autoConnect("AutoConnectAP", "password123")) {
    Serial.println("Verbindung fehlgeschlagen, ESP wird neu gestartet...");
    delay(3000);
    ESP.restart(); // Neustarten, falls die Verbindung fehlschlägt
  }

  // Verbindung erfolgreich hergestellt, IP anzeigen
  Serial.println("Verbunden!");
  Serial.print("IP Adresse: ");
  Serial.println(WiFi.localIP());
}

void loop() {

}
