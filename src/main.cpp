#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "html_pages.h" // Bindet das ausgelagerte HTML & CSS ein

// Zugangsdaten für das eigene WLAN-Netz des ESP32
const char* ssid = "Mein_Autarker_Server";
const char* password = "12345678";

// Statische IP-Konfiguration
IPAddress local_ip(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

// Webserver bleibt direkt in der main
WebServer server(80);

// Konfigurierbare Schwellwerte
float limit_low_red = 2.8f;
float limit_high_red = 3.65f;

// Handler für Seite 1
void handleRoot() {
  String page = buildDashboardPage(limit_low_red, limit_high_red);
  server.send(200, "text/html", page);
}

// Handler für Seite 2
void handleSettings() {
  if (server.hasArg("low") && server.hasArg("high")) {
    limit_low_red = server.arg("low").toFloat();
    limit_high_red = server.arg("high").toFloat();
  }
  String page = buildSettingsPage(limit_low_red, limit_high_red);
  server.send(200, "text/html", page);
}

void setup() {
  Serial.begin(115200);
  delay(500);
  
  // AP-Modus aktivieren
  WiFi.softAPConfig(local_ip, gateway, subnet);
  WiFi.softAP(ssid, password);

  // Routen registrieren
  server.on("/", handleRoot);
  server.on("/settings", handleSettings);

  server.begin();
  Serial.println("AP gestartet.");
  Serial.print("Verbinde mit WLAN: ");
  Serial.println(ssid);
  Serial.print("IP-Adresse: ");
  Serial.println(WiFi.softAPIP());

  // Platz für spätere Module:
  // setupCAN();
  // setupRTC();
  // setupSD();
}

void loop() {
  server.handleClient();

  // Platz für spätere Module:
  // readCAN();
}