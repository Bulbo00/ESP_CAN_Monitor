#include "bms_data.h"

static BmsData bms;
static portMUX_TYPE bmsMux = portMUX_INITIALIZER_UNLOCKED;

static inline uint16_t u16(const uint8_t *d, int i) { return d[i] | (d[i + 1] << 8); }
static inline int16_t  s16(const uint8_t *d, int i) { return (int16_t)u16(d, i); }

// 0x261: 5 Zellen je Frame (Motorola, 11 Bit, Faktor 4 mV), Mux 1..40
static void decodeCells(const uint8_t *d) {
  uint8_t m = d[0];
  if (m < 1 || m > 40) return;
  uint16_t raw[5] = {
    (uint16_t)(((d[1] & 0x7F) << 4) | (d[2] >> 4)),
    (uint16_t)(((d[2] & 0x0F) << 7) | (d[3] >> 1)),
    (uint16_t)(((d[3] & 0x01) << 10) | (d[4] << 2) | (d[5] >> 6)),
    (uint16_t)(((d[5] & 0x3F) << 5) | (d[6] >> 3)),
    (uint16_t)(((d[6] & 0x07) << 8) | d[7])
  };
  for (int k = 0; k < 5; k++) {
    int idx = (m - 1) * 5 + k;
    if (idx < BMS_CELLS) bms.cell_mV[idx] = raw[k] * 4;
  }
}

void bmsDecode(uint32_t id, const uint8_t *d, uint8_t len) {
  if (len < 8 || id < 0x251 || id > 0x261) return;
  bool hit = true;
  portENTER_CRITICAL(&bmsMux);
  switch (id) {
    case 0x251:                                   // BMS_ADS01
      if (d[0] == 54) {
        bms.errSD = u16(d, 1); bms.errAct = u16(d, 3); bms.warn = u16(d, 5);
        bms.hvPlus = d[7] & 0x01; bms.hvPre = d[7] & 0x02;
        bms.hvMinus = d[7] & 0x04; bms.mainRelay = d[7] & 0x08;
        bms.heater = d[7] & 0x10;
      }
      break;
    case 0x252:                                   // BMS_ADS02
      if (d[0] == 58) {
        bms.voltage_V = u16(d, 1) * 0.1f;
        bms.current_A = s16(d, 3) * 0.1f;
      }
      break;
    case 0x253:                                   // BMS_ADS03
      if (d[0] == 51) bms.soc_pct = u16(d, 1) * 0.1f;
      break;
    case 0x254:                                   // BMS_ADS04
      if (d[0] == 52) {
        bms.tAvg_C = s16(d, 1) * 0.1f; bms.tMin_C = s16(d, 3) * 0.1f; bms.tMax_C = s16(d, 5) * 0.1f;
      }
      break;
    case 0x257:                                   // BMS_ADS07
      if (d[0] == 68) {
        bms.isoAlarm1 = d[1] & 0x01; bms.isoAlarm2 = d[1] & 0x02;
        bms.isoRes_kOhm = u16(d, 2);
      }
      break;
    case 0x258:                                   // BMS_ADS08
      if (d[0] == 24) { bms.uAvg_mV = u16(d, 1); bms.uMin_mV = u16(d, 3); bms.uMax_mV = u16(d, 5); }
      break;
    case 0x261: decodeCells(d); break;            // BMS_CellVoltage
    default: hit = false;
  }
  if (hit) bms.lastRx_ms = millis();
  portEXIT_CRITICAL(&bmsMux);
}

void bmsGet(BmsData &out) {
  portENTER_CRITICAL(&bmsMux);
  out = bms;
  portEXIT_CRITICAL(&bmsMux);
}

String bmsJson() {
  BmsData b; bmsGet(b);
  bool online = b.lastRx_ms && (millis() - b.lastRx_ms < 2000);
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
  s.reserve(1800);
  for (int i = 0; i < BMS_CELLS; i++) { if (i) s += ','; s += b.cell_mV[i]; }
  s += "]}";
  return s;
}