#include <Arduino.h>
#include <RTClib.h>
#include "./model.h"

#define RTC_ADDR 0x68

#ifndef SG_MCU_RTC_CORE_H
#define SG_MCU_RTC_CORE_H

class RtcCore {
public:
  static RtcCore *instance() {
    if (!s_instance)
      s_instance = new RtcCore;
    return s_instance;
  }

  RtcCore();

  DateTime getDate();

  DateTime getUtcDate();

  DateTime setDate(DateTime dt);

  bool isRtcRunning();

private:
  static RtcCore *s_instance;
  RtcModel *rtcModel;
  RTC_DS1307 hwRtc;
  RTC_Millis swRtc;
};

RtcCore *RtcCore::s_instance = nullptr;

RtcCore::RtcCore() {
  hwRtc.begin();
  swRtc.adjust(DateTime(2019, 01, 01));
  rtcModel = new RtcModel;
  if (isRtcRunning()) {
    Debug::Print("RTC status: running");
  }
  else {
    Debug::Print("RTC status: not running, instantiate softRtc...");
  }
}

DateTime RtcCore::getDate() {
  RtcSchema rtcSchema = rtcModel->get();
  DateTime utcDt = isRtcRunning() ? hwRtc.now() : swRtc.now();
  TimeSpan tzTimeSpan(rtcSchema.tzOffsetHour * 3600 + rtcSchema.tzOffsetMin * 30);
  return utcDt + tzTimeSpan;
}

DateTime RtcCore::getUtcDate() {
  return isRtcRunning() ? hwRtc.now() : swRtc.now();
}

bool RtcCore::isRtcRunning() {
  Wire.beginTransmission(RTC_ADDR);
  return (Wire.endTransmission() == 0);
}

DateTime RtcCore::setDate(DateTime dt) {
  hwRtc.adjust(dt);
  swRtc.adjust(dt);
  return dt;
}


#endif //SG_MCU_RTC_CORE_H
