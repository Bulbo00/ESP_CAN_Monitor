#include "rtc_clock.h"
#include <Wire.h>
#include <DS3231.h>

static DS3231 rtc;

void rtcInit() {
  rtc.begin();
}

bool rtcIsValid() {
  return rtc.getDateTime().year >= 2025;   // nach Stromverlust ohne Batterie startet die DS3231 im Jahr 2000
}

void rtcSetDateTime(uint16_t y, uint8_t mo, uint8_t d, uint8_t h, uint8_t mi, uint8_t s) {
  rtc.setDateTime(y, mo, d, h, mi, s);
}

String rtcZeitString() {
  RTCDateTime dt = rtc.getDateTime();
  char buf[20];
  snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d",
           dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);
  return String(buf);
}