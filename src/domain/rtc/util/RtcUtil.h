//
// Created by Alfarie-MBP on 2019-08-08.
//
#include <RTClib.h>
#ifndef SG_MCU_RTCUTIL_H
#define SG_MCU_RTCUTIL_H

String DateTimeToIsoString(DateTime dt) {
  char isoString[25];
  sprintf(isoString, "%d-%d-%d %d:%d:%d", dt.year(), dt.month(), dt.day(), dt.hour(), dt.minute(), dt.second());
  return isoString;
}

#endif //SG_MCU_RTCUTIL_H
