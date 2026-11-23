# 🔋 ESP32 LFP-BMS Live Monitor (196s1p)

Ein autarker, mobiler Live-Monitor für Lithium-Eisenphosphat-Batteriesysteme (LFP-BMS mit 196 Zellen), basierend auf einem DOIT ESP32 DevKit V1.

Der ESP32 liest die Daten des BMS passiv vom CAN-Bus, spannt im Fahrzeug ein eigenes WLAN auf und stellt ein responsives Web-Dashboard im Dark Mode bereit. Darüber lassen sich alle 196 Zellspannungen, Gesamtwerte und Temperaturen in Echtzeit direkt auf dem Smartphone überwachen – ganz ohne Router oder Internetverbindung.

## 🚀 Features

- **Autarker Access Point:** Der ESP32 agiert als eigener WLAN-Hotspot (`SSID: Mein_Autarker_Server`, statische IP `192.168.4.1`).
- **Echte Live-Daten per CAN-Bus:** Empfang über den internen TWAI-Controller (500 kbit/s, Listen-Only-Modus, also rückwirkungsfrei auf dem Bus). Die Dekodierung der Botschaften folgt der DBC-Datei `Plantos_eCAN_v4.dbc`.
- **Live-Aktualisierung ohne Neuladen:** Die Seiten holen sich per JavaScript (`fetch`) alle 0,5 bis 1 Sekunde neue Werte als JSON. Ein asynchroner Webserver ist dafür nicht nötig.
- **Responsives Web-Dashboard:** Dark-Mode-Design, optimiert für die Ansicht auf Smartphones im Fahrzeug.
- **Vollständiges Zellen-Raster:** Rasteransicht aller 196 Zellen mit Live-Werten.
- **Intelligente Farbcodierung:** Zellen bei Unter- oder Überspannung werden automatisch hervorgehoben (konfigurierbare Schwellwerte).
- **Fahransicht (`/drive`):** Große Anzeige für SoC, Strom, Gesamtspannung und Ø-Temperatur sowie die 10 niedrigsten Zellspannungen, ohne Scrollen.
- **CAN-Monitor (`/can`):** Sniffer-Ansicht mit dem letzten Frame pro CAN-ID, Frame-Zähler und Bus-Diagnose (verpasste Frames, Busfehler).
- **Echtzeituhr (DS3231):** Die Uhr wird einmalig über die Einstellungsseite per Knopfdruck mit der Uhrzeit des Smartphones gestellt und läuft danach batteriegepuffert weiter.
- **Einstellungsmenü (`/settings`):** Alarm-Grenzwerte lassen sich dynamisch anpassen, außerdem wird hier die Uhr gestellt.
- **Modulare Architektur:** CAN-Empfang, BMS-Dekodierung, RTC, Buzzer und HTML-Seiten sind in eigene Module getrennt. Der CAN-Empfang läuft in einem eigenen FreeRTOS-Task, damit beim Ausliefern der Webseiten keine Frames verloren gehen.

## 📡 Dekodierte BMS-Werte

Aus den CAN-Botschaften `0x251`–`0x258` und `0x261` werden aktuell ausgewertet:

- Gesamtspannung, Strom, SoC
- Temperatur (Ø / min / max)
- Alle 196 Einzelzellspannungen sowie Zellspannung Ø / min / max
- Isolationswiderstand und Isolationsalarme
- Schützstatus (HV+, Vorladung, HV−, Hauptrelais) und Batterieheizung
- Fehler- und Warn-Bits (als Rohwerte)

Das Dashboard zeigt davon eine Auswahl, die vollständigen Werte liefert die JSON-Schnittstelle `/api/bms`.

## 🛠️ Hardware-Komponenten

- **Mikrocontroller:** DOIT ESP32 DevKit V1 (Elegoo / Espressif)
- **CAN-Transceiver:** SN65HVD230 (3,3 V-kompatibel, Anbindung an das BMS über CAN-Bus)
- **RTC-Modul:** DS3231 (I²C, mit Pufferbatterie)
- **Buzzer:** für Start- und Warntöne
- **Verbindung:** D-Sub-9-Stecker-Interface (Pin 2: CAN_L, Pin 7: CAN_H, CAN_GND)

### Pinbelegung

| Funktion | GPIO |
|---|---|
| CAN TX (zum SN65HVD230, TXD) | 5 |
| CAN RX (vom SN65HVD230, RXD) | 4 |
| RTC SDA | 21 |
| RTC SCL | 22 |
| Buzzer | 26 |

> Hinweis: Am CAN-Bus müssen an beiden Enden 120 Ω Abschlusswiderstände sitzen. Der Monitor arbeitet im Listen-Only-Modus und benötigt mindestens einen weiteren aktiven Teilnehmer am Bus, der die Frames bestätigt.

## 💻 Software & Entwicklungsumgebung

- **IDE:** Visual Studio Code mit PlatformIO (Git muss installiert sein, damit PlatformIO die Bibliothek laden kann)
- **Framework:** Arduino-Framework (C/C++) auf dem ESP32
- **Bibliotheken:**
  - `WiFi.h` (Netzwerk & Access Point)
  - `WebServer.h` (synchroner HTTP-Server)
  - `driver/twai.h` (ESP-IDF TWAI-Treiber für den CAN-Controller)
  - `Wire.h` (I²C)
  - [Arduino-DS3231](https://github.com/jarzebski/Arduino-DS3231) von Korneliusz Jarzębski (RTC)

## 📂 Projektstruktur

```
ESP_CAN_Monitor/
├── include/
├── lib/
├── src/
│   ├── main.cpp          # Setup, Routen-Handler, Hauptschleife
│   ├── html_pages.h      # HTML-/CSS-/JS-Seiten (Dashboard, Fahransicht, CAN-Monitor, Einstellungen)
│   ├── can_monitor.h/.cpp # TWAI-Init, CAN-Empfangstask, Frame-Tabelle, JSON für den Sniffer
│   ├── bms_data.h/.cpp   # Dekodierung der BMS-Botschaften (nach DBC), JSON für /api/bms
│   ├── rtc_clock.h/.cpp  # DS3231: Zeit lesen, Zeit setzen, Gültigkeitsprüfung
│   └── buzzer.h/.cpp     # Buzzer-Ansteuerung
├── test/
└── platformio.ini        # PlatformIO-Konfiguration
```

## 🌐 Web-Oberfläche und Schnittstellen

| Pfad | Beschreibung |
|---|---|
| `/` | Dashboard mit allen 196 Zellen |
| `/drive` | Fahransicht (große Werte, 10 niedrigste Zellen, SD-Buttons) |
| `/can` | CAN-Monitor (letzter Frame pro ID) |
| `/settings` | Schwellwerte und Uhr stellen |
| `/api/bms` | Dekodierte BMS-Werte als JSON |
| `/api/can` | Rohe CAN-Frames und Bus-Status als JSON |
| `/api/time` | Aktuelle RTC-Zeit und Gültigkeit als JSON |
| `/settime` | Setzt die RTC (Parameter `y`, `mo`, `d`, `h`, `mi`, `s`) |

## ⚙️ Installation & Inbetriebnahme

1. Repository klonen oder herunterladen:

   ```bash
   git clone https://github.com/DEIN-BENUTZERNAME/ESP32-BMS-Monitor.git
   ```

2. Projekt in VS Code öffnen (mit installierter PlatformIO-Erweiterung). Die `platformio.ini` sollte so aussehen:

   ```ini
   [env:esp32doit-devkit-v1]
   platform = espressif32
   board = esp32doit-devkit-v1
   framework = arduino
   lib_deps =
       https://github.com/jarzebski/Arduino-DS3231.git
   ```

3. ESP32 per USB-Kabel verbinden.
4. Code kompilieren und hochladen (Häkchen- und Pfeil-Symbol in der blauen PlatformIO-Statusleiste).
5. Mit dem ESP32 verbinden:
   - WLAN-Netzwerk am Smartphone suchen: `Mein_Autarker_Server`
   - Passwort: `12345678` (vor dem Einsatz im Fahrzeug unbedingt ändern)
6. Dashboard öffnen: Browser auf dem Smartphone starten und `192.168.4.1` aufrufen.
7. **Uhr stellen (einmalig):** Seite **Schwellwerte** öffnen und auf „Zeit vom Handy übernehmen" tippen. Die DS3231 behält die Zeit danach dank der Pufferbatterie. Zeigt die Seite nach einem Neustart „ungültig" an, ist die Batterie leer oder fehlt.

## ⚠️ Hinweise

- Der Monitor **liest nur** und sendet nichts auf den Fahrzeugbus.
- Das Projekt ist eine Anzeige- und Protokollhilfe und kein Ersatz für die Schutzfunktionen des BMS.
- Die Vorzeichenkonvention des Stroms (Laden/Entladen) ergibt sich aus der DBC-Datei und sollte im Fahrbetrieb überprüft werden.
- Wird die `platform` in der `platformio.ini` nicht auf eine feste Version gesetzt, kann ein Platform-Update den Arduino-Core wechseln und ältere Bibliotheken (z. B. `AsyncTCP-esphome`) unbrauchbar machen.

## 🗺️ Roadmap / Nächste Schritte

- [x] Stabiler ESP32 Access Point & lokaler Webserver
- [x] Responsives HTML-Dashboard für 196 Zellen im Raster
- [x] Farbliche Status-Indikatoren und Einstellungsseite für Schwellwerte
- [x] CAN-Bus-Empfang (TWAI-Treiber) mit Dekodierung echter Messwerte
- [x] Live-Aktualisierung der Seiten per JSON
- [x] Integration einer Echtzeituhr (RTC), Zeit per Smartphone einstellbar
- [ ] Anbindung einer SD-Karten-Protokollierung (Data Logging, Buttons in der Fahransicht sind schon vorbereitet)
- [ ] Schwellwerte dauerhaft speichern (aktuell nach Neustart wieder auf Standardwerten)
- [ ] Akustische Warnung bei Grenzwertverletzung
- [ ] Weitere Werte im Dashboard (Min-/Max-Zelle, Isolationswiderstand, Fehlerbits)

## 📝 Lizenz

Dieses Projekt ist für den privaten und experimentellen Gebrauch im Bereich E-Mobilität und Energiespeicher konzipiert.
