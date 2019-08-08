//
// Created by Alfarie-MBP on 2019-08-08.
//

#include "rtc_core.h"

RtcCore::RtcCore() {
  swRtc.adjust(DateTime(2019, 01, 01));
}

DateTime RtcCore::getDate() {
  return hwRtc.isrunning() ? hwRtc.now() : swRtc.now();
 }