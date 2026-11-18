#include "rtc_clock.h"
#include <Wire.h>
#include <DS3231.h>
#include <Preferences.h>

static DS3231 rtc;
static Preferences prefs;

static void syncRtcOnNewFlash() {
  String build = String(__DATE__) + " " + String(__TIME__);
  prefs.begin("rtc", false);
  if (prefs.getString("build", "") != build) {
    rtc.setDateTime(__DATE__, __TIME__);
    prefs.putString("build", build);
  }
  prefs.end();
}

void rtcInit() {
  rtc.begin();
  syncRtcOnNewFlash();
}

String rtcZeitString() {
  RTCDateTime dt = rtc.getDateTime();
  char buf[20];
  snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d",
           dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);
  return String(buf);
}