# 🔋 ESP32 LFP-BMS Live Monitor (196s1p)

Ein autarker, mobiler Live-Monitor für Lithium-Eisenphosphat-Batteriesysteme (LFP-BMS mit 196 Zellen), basierend auf einem **DOIT ESP32 DevKit V1**. 

Das System spannt im Fahrzeug ein eigenes WLAN auf und stellt ein responsives Web-Dashboard im Dark Mode bereit, über das alle 196 Zellspannungen, Gesamtwerte und Temperaturen in Echtzeit direkt auf dem Smartphone überwacht werden können – ganz ohne Router oder Internetverbindung.

---

## 🚀 Features

* **Autarker Access Point:** Der ESP32 agiert als eigener WLAN-Hotspot (`SSID: Mein_Autarker_Server`, statische IP `192.168.4.1`).
* **Responsives Web-Dashboard:** Modernes Dark-Mode-Design, optimiert für die Ansicht auf Smartphones im Fahrzeug.
* **Vollständiges Zellen-Raster:** Übersetzer- und Gitternschansicht für alle **196 Zellen** mit Live-Werten.
* **Intelligente Farbcodierung:** Automatische visuelle Hervorhebung von Zellen bei Unterspannung oder Überspannung (konfigurierbare Schwellwerte).
* **Einstellungsmenü:** Über eine integrierte Web-Oberfläche (`/settings`) lassen sich Alarm-Grenzwerte dynamisch anpassen.
* **Modulare Architektur:** Schlanke, stabile Code-Struktur für einfache Erweiterbarkeit (Vorbereitet für spätere CAN-Bus-, SD-Karten- und RTC-Integration).

---

## 🛠️ Hardware-Komponenten

* **Mikrokontroller:** DOIT ESP32 DevKit V1 (Elegoo / Espressif)
* **CAN-Transceiver:** SN65HVD230 (3.3V-kompatibel für die Anbindung an das BMS über CAN-Bus)
* **Verbindung:** D-Sub-9-Stecker-Interface (Pin 2: CAN_L, Pin 7: CAN_H, CAN_GND)

---

## 💻 Software & Entwicklungsumgebung

* **IDE:** Visual Studio Code mit **PlatformIO**
* **Framework:** Arduino-Framework (C/C++)
* **Bibliotheken:** 
  * `WiFi.h` (Netzwerk & Access Point)
  * `WebServer.h` (HTTP-Server Handling)

---

## 📂 Projektstruktur (Aktueller Stand)

```text
ESP_CAN_Monitor/
├── .pio/
├── include/
├── src/
│   └── main.cpp          # Enthält Webserver, AP-Logik und HTML-Dashboard
├── test/
└── platformio.ini        # PlatformIO Konfigurationsdatei

```

---

## ⚙️ Installation & Inbetriebnahme

1. **Repository klonen oder herunterladen:**
```bash
git clone [https://github.com/DEIN-BENUTZERNAME/ESP32-BMS-Monitor.git](https://github.com/DEIN-BENUTZERNAME/ESP32-BMS-Monitor.git)

```


2. **Projekt in VS Code öffnen** (mit installierter PlatformIO-Erweiterung).
3. **ESP32 per USB-Kabel verbinden** (Stelle sicher, dass das Board als `esp32doit-devkit-v1` in der `platformio.ini` konfiguriert ist).
4. **Code kompilieren und hochladen** (Häkchen- und Pfeil-Symbol in der blauen PlatformIO-Statusleiste).
5. **Mit dem ESP32 verbinden:**
* WLAN-Netzwerk am Smartphone suchen: **`Mein_Autarker_Server`**
* Passwort: **`12345678`**


6. **Dashboard öffnen:**
* Browser auf dem Smartphone öffnen und die IP **`192.168.4.1`** aufrufen.



---

## 🗺️ Roadmap / Nächste Schritte

* [x] Stabiler ESP32 Access Point & lokaler Webserver
* [x] Responsives HTML-Dashboard für 196 Zellen im Raster
* [x] Farbliche Status-Indikatoren und Einstellungsseite für Schwellwerte
* [ ] Integration des CAN-Bus-Empfangs (TWAI-Treiber) zur Verknüpfung echter Messwerte
* [ ] Anbindung einer SD-Karten-Protokollierung (Data Logging)
* [ ] Integration einer Echtzeituhr (RTC) für Zeitstempel

---

## 📝 Lizenz

Dieses Projekt ist für den privaten und experimentellen Gebrauch im Bereich E-Mobilität und Energiespeicher konzipiert.

```

```
