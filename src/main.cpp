#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// Zugangsdaten für das eigene WLAN-Netz des ESP32
const char* ssid = "Mein_Autarker_Server";
const char* password = "12345678";

// Statische IP-Konfiguration
IPAddress local_ip(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

WebServer server(80);

// Konfigurierbare Schwellwerte (könnten später auch auf SD/SPIFFS gespeichert werden)
float limit_low_red = 2.8f;     // Unterhalb davon: Rot (Unterspannung)
float limit_high_red = 3.65f;   // Oberhalb davon: Rot (Überladung/Voll)
// Dazwischen: Grün / Normalbereich

// Gemeinsames CSS und Header-Navigation für beide Seiten
String getHeader(String title) {
  String h = "<!DOCTYPE html><html><head>";
  h += "<meta charset='utf-8'>";
  h += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  h += "<title>" + title + "</title>";
  h += "<style>";
  h += "body { font-family: Arial, sans-serif; background-color: #121212; color: #e0e0e0; margin: 0; padding: 15px; }";
  h += "h1, h2 { color: #03dac6; text-align: center; }";
  // Navigationsleiste oben
  h += ".navbar { display: flex; justify-content: center; gap: 15px; margin-bottom: 20px; }";
  h += ".nav-btn { background: #333; color: #03dac6; padding: 8px 16px; border-radius: 5px; text-decoration: none; font-weight: bold; border: 1px solid #03dac6; }";
  h += ".nav-btn:hover, .nav-btn.active { background: #03dac6; color: #121212; }";
  // Dashboard Karten & Raster
  h += ".card-container { display: flex; flex-wrap: wrap; gap: 10px; justify-content: center; margin-bottom: 20px; }";
  h += ".card { background: #1e1e1e; padding: 12px; border-radius: 8px; width: 130px; box-shadow: 0 4px 6px rgba(0,0,0,0.3); text-align: center; }";
  h += ".card h3 { margin: 0 0 5px 0; color: #bb86fc; font-size: 14px; }";
  h += ".card p { font-size: 18px; font-weight: bold; margin: 0; color: #ffffff; }";
  h += ".grid-container { display: grid; grid-template-columns: repeat(auto-fill, minmax(70px, 1fr)); gap: 6px; max-width: 900px; margin: 0 auto; }";
  
  // Dynamische Zellfarben (Standard, Warnung/Rot, Normal/Grün)
  h += ".cell-box { padding: 6px; border-radius: 4px; text-align: center; border: 1px solid #444; }";
  h += ".cell-normal { background: #1b3b2b; color: #2ecc71; }"; // Dunkelgrünlicher BG bei Normalspannung
  h += ".cell-alert { background: #4a1515; color: #e74c3c; }"; // Dunkelroter BG bei Alarm (zu niedrig/hoch)
  h += ".cell-id { color: #aaa; font-size: 9px; }";
  h += ".cell-voltage { font-size: 11px; font-weight: bold; }";
  // Formulare für Einstellungen
  h += "form { max-width: 400px; margin: 0 auto; background: #1e1e1e; padding: 20px; border-radius: 8px; }";
  h += "label { display: block; margin-top: 10px; color: #bb86fc; }";
  h += "input { width: 100%; padding: 8px; margin-top: 5px; background: #2c2c2c; border: 1px solid #444; color: #fff; border-radius: 4px; box-sizing: border-box; }";
  h += "button { margin-top: 15px; width: 100%; background: #03dac6; color: #121212; border: none; padding: 10px; font-weight: bold; border-radius: 4px; cursor: pointer; }";
  h += "</style></head><body>";
  
  // Navigation einfügen
  h += "<div class='navbar'>";
  h += "<a href='/' class='nav-btn'>Live Dashboard</a>";
  h += "<a href='/settings' class='nav-btn'>Schwellwerte</a>";
  h += "</div>";
  return h;
}

// 1. Hauptseite: Live Dashboard mit allen 196 Zellen & Farb-Logik
void handleRoot() {
  String html = getHeader("BMS Live Dashboard");
  html += "<h1>196s1p LFP-BMS Live Monitor</h1>";
  
  // Pack-Übersichtskarten
  html += "<div class='card-container'>";
  html += "<div class='card'><h3>Gesamt</h3><p>652.4 V</p></div>";
  html += "<div class='card'><h3>Strom</h3><p>+12.5 A</p></div>";
  html += "<div class='card'><h3>SoC</h3><p>84 %</p></div>";
  html += "<div class='card'><h3>Temp</h3><p>24.5 °C</p></div>";
  html += "</div>";

  // Zellen-Raster (1 bis 196)
  html += "<h2 style='font-size: 16px;'>Zellspannungen (1 - 196)</h2>";
  html += "<div class='grid-container'>";
  
  // Simulieren wir für die Demonstration verschiedene Spannungen, um die Farben zu zeigen
  for (int i = 1; i <= 196; i++) {
    // Beispiel-Werte erzeugen (Zelle 10 und 50 absichtlich im roten Bereich zum Testen)
    float dummyVoltage = 3.325f;
    if (i == 10) dummyVoltage = 2.75f;   // Unterspannung (Rot)
    if (i == 50) dummyVoltage = 3.80f;   // Überspannung (Rot)

    // Farb-Klasse je nach Schwellwert bestimmen
    String colorClass = "cell-normal";
    if (dummyVoltage <= limit_low_red || dummyVoltage >= limit_high_red) {
      colorClass = "cell-alert";
    }

    html += "<div class='cell-box " + colorClass + "'>";
    html += "<div class='cell-id'>Z. " + String(i) + "</div>";
    html += "<div class='cell-voltage'>" + String(dummyVoltage, 3) + "V</div>";
    html += "</div>";
  }
  
  html += "</div></body></html>";
  server.send(200, "text/html", html);
}

// 2. Einstellungsseite: Schwellwerte anpassen
void handleSettings() {
  // Wenn Formulardaten übergeben wurden, Werte aktualisieren
  if (server.hasArg("low") && server.hasArg("high")) {
    limit_low_red = server.arg("low").toFloat();
    limit_high_red = server.arg("high").toFloat();
  }

  String html = getHeader("BMS Einstellungen");
  html += "<h1>BMS Schwellwerte</h1>";
  html += "<form action='/settings' method='POST'>";
  html += "<label>Unterer Alarm-Grenzwert (Rot in V):</label>";
  html += "<input type='number' step='0.01' name='low' value='" + String(limit_low_red) + "'>";
  
  html += "<label>Oberer Alarm-Grenzwert (Rot in V):</label>";
  html += "<input type='number' step='0.01' name='high' value='" + String(limit_high_red) + "'>";
  
  html += "<button type='submit'>Speichern</button>";
  html += "</form>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
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
}

void loop() {
  server.handleClient();
}