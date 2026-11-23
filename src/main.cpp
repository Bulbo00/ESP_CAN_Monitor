#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <DS3231.h>
#include <Preferences.h>

#include "can_monitor.h"  
#include "buzzer.h"
#include "rtc_clock.h"
#include "html_pages.h"
#include "bms_data.h"

// Zugangsdaten für das eigene WLAN-Netz des ESP32
const char* ssid = "Mein_Autarker_Server";
const char* password = "12345678";

const int buzzerPin = 26;
#define TX_PIN  GPIO_NUM_5
#define RX_PIN  GPIO_NUM_4


// Statische IP-Konfiguration
IPAddress local_ip(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

WebServer server(80);

// Konfigurierbare Schwellwerte
float limit_low_red = 2.8f;
float limit_high_red = 3.65f;

// Status-Variable für spätere SD-Karten-Aufnahme
bool sd_is_recording = false;

// Route 1: Vollständiges Dashboard (/)
void handleRoot() {
  String page = buildDashboardPage(limit_low_red, limit_high_red);
  server.send(200, "text/html", page);
}

// Route 2: Fahransicht (/drive)
void handleDrive() {
  String page = buildDrivePage(sd_is_recording);
  server.send(200, "text/html", page);
}

// Route 3: Einstellungsseite (/settings)
void handleSettings() {
  if (server.hasArg("low") && server.hasArg("high")) {
    limit_low_red = server.arg("low").toFloat();
    limit_high_red = server.arg("high").toFloat();
  }
  String page = buildSettingsPage(limit_low_red, limit_high_red);
  server.send(200, "text/html", page);
}

// Funktion für Live Werte
void handleApiBms() { server.send(200, "application/json", bmsJson()); }

// Hilfs-Routen für SD-Karten Steuerung via Buttons
void handleSDStart() {
  sd_is_recording = true;
  Serial.println("SD-Karten Aufnahme gestartet!");
  server.sendHeader("Location", "/drive", true);
  server.send(302, "text/plain", ""); // Leitet den Browser sofort zurück zur Fahransicht
}

void handleSDStop() {
  sd_is_recording = false;
  Serial.println("SD-Karten Aufnahme gestoppt!");
  server.sendHeader("Location", "/drive", true);
  server.send(302, "text/plain", ""); // Leitet den Browser sofort zurück zur Fahransicht
}

void handleCan()    { server.send(200, "text/html", buildCanPage()); }
void handleApiCan() { server.send(200, "application/json", canJson()); }

//Für RTC- wird über Handy gestellt
void handleApiTime() {
  server.send(200, "application/json",
              "{\"t\":\"" + rtcZeitString() + "\",\"valid\":" + String(rtcIsValid() ? 1 : 0) + "}");
}

void handleSetTime() {
  const char *k[] = {"y", "mo", "d", "h", "mi", "s"};
  for (auto key : k) if (!server.hasArg(key)) { server.send(400, "text/plain", "Parameter fehlen"); return; }
  int y = server.arg("y").toInt(),  mo = server.arg("mo").toInt(), d = server.arg("d").toInt();
  int h = server.arg("h").toInt(),  mi = server.arg("mi").toInt(),  s = server.arg("s").toInt();
  if (y < 2025 || y > 2099 || mo < 1 || mo > 12 || d < 1 || d > 31 || h > 23 || mi > 59 || s > 59) {
    server.send(400, "text/plain", "ungueltige Zeit"); return;
  }
  rtcSetDateTime(y, mo, d, h, mi, s);
  server.send(200, "text/plain", rtcZeitString());
}



void setup() {
  Serial.begin(9600);
  Wire.begin();

  buzzerInit(buzzerPin);
  rtcInit();

  delay(500);
 
  // AP-Modus aktivieren
  WiFi.softAPConfig(local_ip, gateway, subnet);
  WiFi.softAP(ssid, password);

  // Routen registrieren
  server.on("/", handleRoot);
  server.on("/drive", handleDrive);
  server.on("/settings", handleSettings);
  server.on("/sd_start", handleSDStart);
  server.on("/sd_stop", handleSDStop);

  server.begin();
  Serial.println("AP gestartet.");
  Serial.print("Verbinde mit WLAN: ");
  Serial.println(ssid);
  Serial.print("IP-Adresse: ");
  Serial.println(WiFi.softAPIP());

  server.on("/api/bms", handleApiBms);

  server.on("/can", handleCan);
  server.on("/api/can", handleApiCan);
  if (!canInit(TX_PIN, RX_PIN)) Serial.println("CAN-Init fehlgeschlagen!");

  server.on("/api/time", handleApiTime);
  server.on("/settime", handleSetTime);


  startup_sound();
}

void loop() {
  //VoltageLow_sound();
  server.handleClient();
  static uint32_t lastPrint = 0;
  if (millis() - lastPrint >= 10000) {
    lastPrint = millis();
    Serial.println(rtcZeitString());
  }
}