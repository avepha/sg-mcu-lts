#include <Arduino.h>
#include <RTClib.h>
#include "./model.h"

#ifndef SG_MCU_RTC_CORE_H
#define SG_MCU_RTC_CORE_H

RTC_DS1307 hwRtc;
RTC_Millis swRtc;

class RtcCore {
public:
  static RtcCore* instance()
  {
    if (!s_instance)
      s_instance = new RtcCore;
    return s_instance;
  }

  RtcCore();
  DateTime getDate();
  DateTime getUtcDate();
  DateTime setDate(DateTime dt);

private:
  static RtcCore *s_instance;
  RtcModel *rtcModel;
};

RtcCore *RtcCore::s_instance = nullptr;

RtcCore::RtcCore() {
  hwRtc.begin();
  swRtc.adjust(DateTime(2019, 01, 01));
  rtcModel = new RtcModel;
}

DateTime RtcCore::getDate() {
  Debug::Print("Rtc isRunning: " + String(hwRtc.isrunning()));
  RtcSchema rtcSchema =  rtcModel->get();
  DateTime utcDt = hwRtc.isrunning() ? hwRtc.now() : swRtc.now();
  TimeSpan tzTimeSpan(rtcSchema.tzOffsetHour * 3600 + rtcSchema.tzOffsetMin * 30);
  return utcDt + tzTimeSpan;
}

DateTime RtcCore::getUtcDate() {
  return hwRtc.isrunning() ? hwRtc.now() : swRtc.now();
}

DateTime RtcCore::setDate(DateTime dt) {
  hwRtc.adjust(dt);
  swRtc.adjust(dt);
  return dt;
}


#endif //SG_MCU_RTC_CORE_H
