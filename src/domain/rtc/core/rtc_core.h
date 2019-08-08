//
// Created by Alfarie-MBP on 2019-08-08.
//
#include <RTClib.h>

#ifndef SG_MCU_RTC_CORE_H
#define SG_MCU_RTC_CORE_H

class RtcCore {
public:
  RtcCore();
  DateTime getDate();

private:
  DateTime now;
  RTC_DS1307 hwRtc;
  RTC_Millis swRtc;
};


#endif //SG_MCU_RTC_CORE_H
