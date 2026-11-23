#pragma once
#include <Arduino.h>

void rtcInit();
String rtcZeitString();   // "2026-11-23 11:58:03"
bool rtcIsValid();        // false, wenn die Uhr nie gestellt wurde oder die Batterie leer war
void rtcSetDateTime(uint16_t y, uint8_t mo, uint8_t d, uint8_t h, uint8_t mi, uint8_t s);