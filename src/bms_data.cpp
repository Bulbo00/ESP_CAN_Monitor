#include "bms_data.h"

static BmsData bms;                                             // Zentrales Datenmodell für den aktuellen Zustand des Batterie-Management-Systems
static portMUX_TYPE bmsMux = portMUX_INITIALIZER_UNLOCKED;      // Spinlock für kritische Abschnitte zwischen den beiden ESP32-Kernen (ISR/CAN-Task vs. Webserver)

// Hilfsfunktionen zum Auslesen von 16-Bit-Werten im Little-Endian-Format (Intel-Bytefolge: LSB an Index i, MSB an i+1)
static inline uint16_t u16(const uint8_t *d, int i) { return d[i] | (d[i + 1] << 8); }
static inline int16_t  s16(const uint8_t *d, int i) { return (int16_t)u16(d, i); } // Vorzeichenbehaftete Interpretation (Zweierkomplement, z. B. für Lade-/Entladestrom)

// Dekodiert die gemultiplexte Einzelzellspannungs-Botschaft (ID 0x261)
// Layout: Byte 0 = Multiplexer-Index (1..40), Bytes 1..7 (56 Bit) = 5 gepackte 11-Bit-Zellwerte im Big-Endian-Format (Motorola), Auflösung 4 mV/LSB
static void decodeCells(const uint8_t *d) {
  uint8_t m = d[0];
  if (m < 1 || m > 40) return; // Gültigkeitsbereich prüfen (40 Mux-Frames * 5 Zellen = max. 200 Zellen)

  // Bit-Slicing über Byte-Grenzen hinweg: Extrahiert fünf lückenlos aneinandergepackte 11-Bit-Rohwerte aus den Bytes d[1]..d[7]
  // (Bit 7 von d[1] bleibt ungenutzt; danach folgen 5 * 11 Bit = 55 Bit)
  uint16_t raw[5] = {
    (uint16_t)(((d[1] & 0x7F) << 4) | (d[2] >> 4)),                      // Zelle 1: Untere 7 Bit aus d[1] + obere 4 Bit aus d[2]
    (uint16_t)(((d[2] & 0x0F) << 7) | (d[3] >> 1)),                      // Zelle 2: Untere 4 Bit aus d[2] + obere 7 Bit aus d[3]
    (uint16_t)(((d[3] & 0x01) << 10) | (d[4] << 2) | (d[5] >> 6)),       // Zelle 3: Bit 0 aus d[3] + komplett d[4] (8 Bit) + obere 2 Bit aus d[5]
    (uint16_t)(((d[5] & 0x3F) << 5) | (d[6] >> 3)),                      // Zelle 4: Untere 6 Bit aus d[5] + obere 5 Bit aus d[6]
    (uint16_t)(((d[6] & 0x07) << 8) | d[7])                              // Zelle 5: Untere 3 Bit aus d[6] + komplett d[7] (8 Bit)
  };

  // Rohwerte auf den globalen Zellen-Index abbilden und mit Skalierungsfaktor 4 mV in Millivolt umrechnen
  for (int k = 0; k < 5; k++) {
    int idx = (m - 1) * 5 + k;
    if (idx < BMS_CELLS) bms.cell_mV[idx] = raw[k] * 4;
  }
}

// Zentrale Parser-Routine: Filtert relevante BMS-CAN-IDs (0x251..0x261) und aktualisiert den Zustand Thread-safe
void bmsDecode(uint32_t id, const uint8_t *d, uint8_t len) {
  if (len < 8 || id < 0x251 || id > 0x261) return; // Fast-Path-Filter: Ignoriert fremde Bus-IDs oder zu kurze Frames sofort

  bool hit = true;
  portENTER_CRITICAL(&bmsMux); // Sperrt Interrupts und den zweiten CPU-Kern gegen gleichzeitigen Speicherzugriff

  switch (id) {
    case 0x251:                                   // BMS_ADS01: Fehler-Bitfelder & Status der Hochvolt-Schütze
      if (d[0] == 54) {                           // Byte 0 fungiert als Sub-ID / Message-Identifier zur Absicherung
        bms.errSD = u16(d, 1); bms.errAct = u16(d, 3); bms.warn = u16(d, 5); // Shutdown-Fehler, aktive Fehler, Warnungen (je 16-Bit-Bitmaske)
        // Status-Bits der Schütze und Heizung aus Byte 7 per Bitmaskierung extrahieren
        bms.hvPlus = d[7] & 0x01; bms.hvPre = d[7] & 0x02;
        bms.hvMinus = d[7] & 0x04; bms.mainRelay = d[7] & 0x08;
        bms.heater = d[7] & 0x10;
      }
      break;

    case 0x252:                                   // BMS_ADS02: Gesamtspannung und Gesamtstrom des Akkupacks
      if (d[0] == 58) {
        bms.voltage_V = u16(d, 1) * 0.1f;         // Auflösung 0,1 V/LSB (unsigned)
        bms.current_A = s16(d, 3) * 0.1f;         // Auflösung 0,1 A/LSB (signed: Vorzeichen gibt Lade-/Entladerichtung an)
      }
      break;

    case 0x253:                                   // BMS_ADS03: Ladezustand (State of Charge)
      if (d[0] == 51) bms.soc_pct = u16(d, 1) * 0.1f; // Auflösung 0,1 %/LSB
      break;

    case 0x254:                                   // BMS_ADS04: Zelltemperaturen (Durchschnitt, Minimum, Maximum)
      if (d[0] == 52) {
        bms.tAvg_C = s16(d, 1) * 0.1f; bms.tMin_C = s16(d, 3) * 0.1f; bms.tMax_C = s16(d, 5) * 0.1f; // Auflösung 0,1 °C/LSB (signed für Minusgrade)
      }
      break;

    case 0x257:                                   // BMS_ADS07: Isolationsüberwachung (IMD / Insulation Monitoring)
      if (d[0] == 68) {
        bms.isoAlarm1 = d[1] & 0x01; bms.isoAlarm2 = d[1] & 0x02; // Isolations-Warnstufe 1 & 2
        bms.isoRes_kOhm = u16(d, 2);                              // Isolationswiderstand in kOhm
      }
      break;

    case 0x258:                                   // BMS_ADS08: Extremwerte der Zellspannungen (bereits vom BMS voraggregiert)
      if (d[0] == 24) { bms.uAvg_mV = u16(d, 1); bms.uMin_mV = u16(d, 3); bms.uMax_mV = u16(d, 5); } // Auflösung 1 mV/LSB
      break;

    case 0x261: decodeCells(d); break;            // BMS_CellVoltage: Gemultiplexte Einzelzellspannungen dekodieren
    default: hit = false;                         // IDs im Bereich 0x255, 0x256, 0x259, 0x260 werden nicht ausgewertet
  }

  if (hit) bms.lastRx_ms = millis();              // Watchdog-Zeitstempel für Online-Status-Erkennung aktualisieren
  portEXIT_CRITICAL(&bmsMux);
}

// Erstellt eine konsistente Kopie der BMS-Struktur für lesende Tasks (z. B. Webserver oder Display)
void bmsGet(BmsData &out) {
  portENTER_CRITICAL(&bmsMux);
  out = bms;
  portEXIT_CRITICAL(&bmsMux);
}

// Serialisiert den gesamten BMS-Zustand inkl. aller Einzelzellspannungen in ein JSON-Objekt
String bmsJson() {
  BmsData b; bmsGet(b); // Atomaren Snapshot ziehen, um den Spinlock nicht während der String-Formatierung zu blockieren
  bool online = b.lastRx_ms && (millis() - b.lastRx_ms < 2000); // Timeout-Überwachung: BMS gilt als offline, wenn > 2 s kein Frame ankam

  // Kopfdaten in festen Stack-Puffer formatieren (vermeidet Heap-Fragmentierung durch viele kleine String-Konkatenationen)
  char head[460];
  snprintf(head, sizeof(head),
    "{\"online\":%d,\"u\":%.1f,\"i\":%.1f,\"soc\":%.1f,"
    "\"tavg\":%.1f,\"tmin\":%.1f,\"tmax\":%.1f,"
    "\"uavg\":%u,\"umin\":%u,\"umax\":%u,"
    "\"iso\":%.0f,\"isoA1\":%d,\"isoA2\":%d,"
    "\"sd\":%u,\"act\":%u,\"wrn\":%u,"
    "\"hv\":[%d,%d,%d,%d],\"heater\":%d,\"cells\":[",
    online, b.voltage_V, b.current_A, b.soc_pct,
    b.tAvg_C, b.tMin_C, b.tMax_C,
    b.uAvg_mV, b.uMin_mV, b.uMax_mV,
    b.isoRes_kOhm, b.isoAlarm1, b.isoAlarm2,
    b.errSD, b.errAct, b.warn,
    b.hvPlus, b.hvPre, b.hvMinus, b.mainRelay, b.heater);

  String s(head);
  s.reserve(1800); // Einmalige Heap-Allokation für das gesamte JSON inkl. Zell-Array (verhindert Reallokationen in der Schleife)
  for (int i = 0; i < BMS_CELLS; i++) { if (i) s += ','; s += b.cell_mV[i]; }
  s += "]}";
  return s;
}