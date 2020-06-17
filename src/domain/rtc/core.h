#include <Arduino.h>
#include <RTClib.h>
#include "./model.h"
#include "./util/RtcTask.h"
#include "./util/RtcUtil.h"
#include "logger/log.h"


#ifndef SG_MCU_RTC_CORE_H
#define SG_MCU_RTC_CORE_H

class RtcCore {
public:
  static RTC_DS1307 hwRtc;
  static RTC_Millis swRtc;

  static RtcCore *instance() {
    if (!s_instance)
      s_instance = new RtcCore;
    return s_instance;
  }

  RtcCore();

  DateTime getDate();

  DateTime getUtcDate();

  DateTime setDate(DateTime dt);

  bool isHwRunning();

private:
  static RtcCore *s_instance;
  RtcModel *rtcModel;
  RtcTask *rtcTask;
};

RtcCore *RtcCore::s_instance = nullptr;

RtcCore::RtcCore() {
  hwRtc.begin();
  swRtc.adjust(DateTime(2019, 01, 01));
  rtcTask = new RtcTask(&hwRtc, &swRtc);
  rtcModel = new RtcModel;
}

DateTime RtcCore::getDate() {
  RtcSchema rtcSchema = rtcModel->get();
  DateTime utcDt = rtcTask->getNow();
  TimeSpan tzTimeSpan(rtcSchema.tzOffsetHour * 3600 + rtcSchema.tzOffsetMin * 30);

  Log::trace("rtc", "call func getDate " + DateTimeToIsoString(utcDt + tzTimeSpan));
  return utcDt + tzTimeSpan;
}

DateTime RtcCore::getUtcDate() {
  Log::trace("rtc", "call func getUtcDate " + DateTimeToIsoString(rtcTask->getNow()));
  return rtcTask->getNow();
}

DateTime RtcCore::setDate(DateTime dt) {
  hwRtc.adjust(dt);
  swRtc.adjust(dt);

  Log::trace("rtc", "call func setDate " + DateTimeToIsoString(dt));
  return dt;
}

bool RtcCore::isHwRunning() {
  return rtcTask->isHwRunning();
}

RTC_DS1307 RtcCore::hwRtc;
RTC_Millis RtcCore::swRtc;
#endif //SG_MCU_RTC_CORE_H
