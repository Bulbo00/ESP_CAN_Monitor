#pragma once
#include <Arduino.h>

#define BMS_CELLS 196

struct BmsData {
  float voltage_V, current_A, soc_pct;
  float tAvg_C, tMin_C, tMax_C;
  uint16_t uAvg_mV, uMin_mV, uMax_mV;
  float isoRes_kOhm;
  bool isoAlarm1, isoAlarm2;
  uint16_t errSD, errAct, warn;
  bool hvPlus, hvPre, hvMinus, mainRelay, heater;
  uint16_t cell_mV[BMS_CELLS];
  uint32_t lastRx_ms;
};

void bmsDecode(uint32_t id, const uint8_t *d, uint8_t len);  // im CAN-Task aufrufen
void bmsGet(BmsData &out);                                   // thread-sicherer Snapshot
String bmsJson();