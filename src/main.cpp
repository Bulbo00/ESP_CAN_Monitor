#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <DS3231.h>
#include <Preferences.h>

#include "buzzer.h"
#include "rtc_clock.h"
#include "html_pages.h"

// Zugangsdaten für das eigene WLAN-Netz des ESP32
const char* ssid = "Mein_Autarker_Server";
const char* password = "12345678";

const int buzzerPin = 4;

DS3231 rtc;
Preferences prefs;

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



void getTime(){
  RTCDateTime  dt = rtc.getDateTime();
  Serial.printf("%4d-%02d-%02d %02d:%02d:%02d\n",
                dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);
}
void syncRtcOnNewFlash() {
  String build = String(__DATE__) + " " + String(__TIME__);
  prefs.begin("rtc", false);
  if (prefs.getString("build", "") != build) {   // neuer Build geflasht
    rtc.setDateTime(__DATE__, __TIME__);
    prefs.putString("build", build);
  }
  prefs.end();
}



void setup() {
  Serial.begin(9600);
  Wire.begin();

  buzzerInit(4);
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

  startup_sound();
}

void loop() {
  server.handleClient();
  //VoltageLow_sound();
  getTime();
  delay(10000);
}