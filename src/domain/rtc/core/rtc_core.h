//
// Created by Alfarie-MBP on 2019-08-08.
//
#include <Arduino.h>
#include <RTClib.h>

#ifndef SG_MCU_RTC_CORE_H
#define SG_MCU_RTC_CORE_H

class RtcCore {
public:
  RtcCore();
  DateTime getDate();
  DateTime setDate(DateTime dt);

private:
  RTC_DS1307 hwRtc;
  RTC_Millis swRtc;
};

RtcCore::RtcCore() {
  swRtc.adjust(DateTime(2019, 01, 01));
}

DateTime RtcCore::getDate() {
  return hwRtc.isrunning() ? hwRtc.now() : swRtc.now();
}

DateTime RtcCore::setDate(DateTime dt) {

  hwRtc.adjust(dt);
  swRtc.adjust(dt);
  return dt;
}


#endif //SG_MCU_RTC_CORE_H
