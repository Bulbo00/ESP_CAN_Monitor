#ifndef HTML_PAGES_H
#define HTML_PAGES_H

#include <Arduino.h>

// Gemeinsames CSS und dynamische Navbar mit aktiver Markierung
String getHeader(String title, String currentPath) {
  String h = "<!DOCTYPE html><html><head>";
  h += "<meta charset='utf-8'>";
  h += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  h += "<title>" + title + "</title>";
  h += "<style>";
  h += "body { font-family: Arial, sans-serif; background-color: #121212; color: #e0e0e0; margin: 0; padding: 10px; box-sizing: border-box; }";
  h += "h1 { color: #03dac6; text-align: center; margin: 8px 0; font-size: 18px; }";
  h += "h2 { color: #03dac6; text-align: center; margin: 5px 0; font-size: 16px; }";
  
  // Navigation (3 Buttons)
  h += ".navbar { display: flex; justify-content: center; gap: 10px; margin-bottom: 12px; }";
  h += ".nav-btn { background: #333; color: #03dac6; padding: 8px 14px; border-radius: 6px; text-decoration: none; font-size: 13px; font-weight: bold; border: 1px solid #03dac6; }";
  h += ".nav-btn.active { background: #03dac6; color: #121212; }";
  
  // Allgemeine Karten (Dashboard)
  h += ".card-container { display: flex; flex-wrap: wrap; gap: 6px; justify-content: center; margin-bottom: 8px; }";
  h += ".card { background: #1e1e1e; padding: 6px; border-radius: 6px; width: 22%; min-width: 70px; text-align: center; box-shadow: 0 2px 4px rgba(0,0,0,0.3); }";
  h += ".card h3 { margin: 0 0 2px 0; color: #bb86fc; font-size: 11px; }";
  h += ".card p { font-size: 13px; font-weight: bold; margin: 0; color: #ffffff; }";
  
  // ==========================================
  // FAHRANSICHT - DEUTLICH VERGRÖßERT (OHNE SCROLLEN)
  // ==========================================
  h += ".drive-cards { display: grid; grid-template-columns: repeat(2, 1fr); gap: 8px; margin-bottom: 10px; }";
  h += ".drive-card { background: #1e1e1e; padding: 10px; border-radius: 8px; text-align: center; border: 1px solid #333; }";
  h += ".drive-card h3 { margin: 0 0 4px 0; color: #bb86fc; font-size: 13px; text-transform: uppercase; }";
  h += ".drive-card p { font-size: 22px; font-weight: bold; margin: 0; color: #ffffff; }";
  
  h += ".section-title { color: #bb86fc; font-size: 13px; margin: 8px 0 6px 0; text-align: center; font-weight: bold; }";
  
  // 10 niedrigste Zellen als größeres Grid (2 Reihen à 5 Zellen)
  h += ".lowest-grid { display: grid; grid-template-columns: repeat(5, 1fr); gap: 6px; max-width: 600px; margin: 0 auto 10px auto; }";
  h += ".low-box { background: #4a1515; border: 2px solid #e74c3c; padding: 8px 4px; border-radius: 6px; text-align: center; }";
  h += ".low-id { color: #f1c40f; font-size: 11px; font-weight: bold; margin-bottom: 2px; }"; // Zellnummer gelb & größer
  h += ".low-val { color: #ffffff; font-size: 15px; font-weight: bold; }"; // Spannungswert groß und weiß

  // SD-Karten Kontroll-Buttons unten in Fahransicht
  h += ".sd-container { display: flex; justify-content: center; gap: 15px; margin-top: 10px; }";
  h += ".sd-btn { padding: 12px 20px; font-size: 14px; font-weight: bold; border: none; border-radius: 6px; cursor: pointer; text-decoration: none; box-shadow: 0 4px 6px rgba(0,0,0,0.3); }";
  h += ".btn-start { background: #27ae60; color: white; }";
  h += ".btn-stop { background: #c0392b; color: white; }";

  // Standard Grid für Voll-Dashboard (196 Zellen)
  h += ".grid-container { display: grid; grid-template-columns: repeat(auto-fill, minmax(65px, 1fr)); gap: 4px; max-width: 900px; margin: 0 auto; }";
  h += ".cell-box { padding: 4px; border-radius: 4px; text-align: center; border: 1px solid #444; }";
  h += ".cell-normal { background: #1b3b2b; color: #2ecc71; }";
  h += ".cell-alert { background: #4a1515; color: #e74c3c; }";
  h += ".cell-id { color: #aaa; font-size: 8px; }";
  h += ".cell-voltage { font-size: 10px; font-weight: bold; }";

  // Formulare (Einstellungen)
  h += "form { max-width: 350px; margin: 0 auto; background: #1e1e1e; padding: 15px; border-radius: 6px; }";
  h += "label { display: block; margin-top: 8px; color: #bb86fc; font-size: 13px; }";
  h += "input { width: 100%; padding: 8px; margin-top: 4px; background: #2c2c2c; border: 1px solid #444; color: #fff; border-radius: 4px; box-sizing: border-box; font-size: 14px; }";
  h += "button[type='submit'] { margin-top: 15px; width: 100%; background: #03dac6; color: #121212; border: none; padding: 10px; font-weight: bold; border-radius: 6px; cursor: pointer; font-size: 14px; }";
  h += "</style></head><body>";
  
  // Navbar generieren
  h += "<div class='navbar'>";
  h += "<a href='/' class='nav-btn " + String(currentPath == "/" ? "active" : "") + "'>Dashboard</a>";
  h += "<a href='/drive' class='nav-btn " + String(currentPath == "/drive" ? "active" : "") + "'>Fahransicht</a>";
  h += "<a href='/settings' class='nav-btn " + String(currentPath == "/settings" ? "active" : "") + "'>Schwellwerte</a>";
  h += "</div>";
  return h;
}

// DRITTE SEITE: Fahransicht (Groß, übersichtlich, ohne Scrollen)
String buildDrivePage(bool sd_recording) {
  String html = getHeader("BMS Fahransicht", "/drive");
  html += "<h1>Fahr-Live-Ansicht</h1>";
  
  // 1. Haupt-Messwerte als 2x2 großes Grid für maximale Lesbarkeit
  html += "<div class='drive-cards'>";
  html += "<div class='drive-card'><h3>SoC</h3><p>84 %</p></div>";
  html += "<div class='drive-card'><h3>Akt. Strom</h3><p>-35.2 A</p></div>";
  html += "<div class='drive-card'><h3>Verbrauch</h3><p>1.2 Ah</p></div>";
  html += "<div class='drive-card'><h3>Ø Temperatur</h3><p>26.5 °C</p></div>";
  html += "</div>";

  // 2. Die 10 niedrigsten Spannungswerte (Deutlich vergrößerte Boxen & Zahlen)
  html += "<div class='section-title'>⚠️ Top 10 Niedrigste Zellspannungen</div>";
  html += "<div class='lowest-grid'>";
  
  int lowestCells[10] = { 12, 45, 89, 3, 112, 150, 77, 134, 5, 180 };
  float lowestVoltages[10] = { 3.120f, 3.135f, 3.140f, 3.142f, 3.150f, 3.155f, 3.160f, 3.162f, 3.165f, 3.170f };

  for(int i = 0; i < 10; i++) {
    html += "<div class='low-box'>";
    html += "<div class='low-id'>Z. " + String(lowestCells[i]) + "</div>";
    html += "<div class='low-val'>" + String(lowestVoltages[i], 3) + "V</div>";
    html += "</div>";
  }
  html += "</div>";

  // 3. Statusanzeige & SD-Karten Steuerungsbuttons unten
  html += "<div style='text-align: center; font-size: 12px; margin-bottom: 6px;'>SD-Status: ";
  if(sd_recording) {
    html += "<span style='color: #2ecc71; font-weight: bold;'>● AUFNAHME LÄUFT</span>";
  } else {
    html += "<span style='color: #e74c3c; font-weight: bold;'>Bereit (Gestoppt)</span>";
  }
  html += "</div>";

  html += "<div class='sd-container'>";
  html += "<a href='/sd_start' class='sd-btn btn-start'>▶ SD Start</a>";
  html += "<a href='/sd_stop' class='sd-btn btn-stop'>⏹ SD Stopp</a>";
  html += "</div>";

  html += "</body></html>";
  return html;
}

// 1. Hauptseite: Vollständiges Dashboard (196 Zellen)
String buildDashboardPage(float limit_low, float limit_high) {
  String html = getHeader("BMS Live Dashboard", "/");
  html += "<h1>196s1p LFP-BMS Live Monitor</h1>";
  
  html += "<div class='card-container'>";
  html += "<div class='card' style='width:22%'><h3>Gesamt</h3><p>652.4 V</p></div>";
  html += "<div class='card' style='width:22%'><h3>Strom</h3><p>+12.5 A</p></div>";
  html += "<div class='card' style='width:22%'><h3>SoC</h3><p>84 %</p></div>";
  html += "<div class='card' style='width:22%'><h3>Temp</h3><p>24.5 °C</p></div>";
  html += "</div>";

  html += "<h2 style='font-size: 14px;'>Zellspannungen (1 - 196)</h2>";
  html += "<div class='grid-container'>";
  
  for (int i = 1; i <= 196; i++) {
    float dummyVoltage = 3.325f;
    if (i == 10) dummyVoltage = 2.75f;
    if (i == 50) dummyVoltage = 3.80f;

    String colorClass = "cell-normal";
    if (dummyVoltage <= limit_low || dummyVoltage >= limit_high) {
      colorClass = "cell-alert";
    }

    html += "<div class='cell-box " + colorClass + "'>";
    html += "<div class='cell-id'>Z." + String(i) + "</div>";
    html += "<div class='cell-voltage'>" + String(dummyVoltage, 3) + "V</div>";
    html += "</div>";
  }
  
  html += "</div></body></html>";
  return html;
}

// 2. Einstellungsseite
String buildSettingsPage(float limit_low, float limit_high) {
  String html = getHeader("BMS Einstellungen", "/settings");
  html += "<h1>Schwellwerte</h1>";
  html += "<form action='/settings' method='POST'>";
  html += "<label>Unterer Alarm (Rot in V):</label>";
  html += "<input type='number' step='0.01' name='low' value='" + String(limit_low) + "'>";
  
  html += "<label>Oberer Alarm (Rot in V):</label>";
  html += "<input type='number' step='0.01' name='high' value='" + String(limit_high) + "'>";
  
  html += "<button type='submit'>Speichern</button>";
  html += "</form>";
  html += "</body></html>";
  return html;
}

#endif