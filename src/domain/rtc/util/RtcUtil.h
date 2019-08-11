//
// Created by Alfarie-MBP on 2019-08-08.
//
#include <RTClib.h>
#ifndef SG_MCU_RTCUTIL_H
#define SG_MCU_RTCUTIL_H

String DateTimeToIsoString(DateTime dt) {
  char isoString[25];
  sprintf(isoString, "%d-%d-%dT%d:%d:%d.000Z", dt.year(), dt.month(), dt.day(), dt.hour(), dt.minute(), dt.second());
  return isoString;
}

DateTime IsoStringToDateTime(String isoString) {
  String date = isoString.substring(0, isoString.indexOf('T'));
  String time = isoString.substring(isoString.indexOf('T') + 1);
  long year = date.substring(0, 4).toInt();
  long month = date.substring(5, 7).toInt();
  long day = date.substring(8, 10).toInt();
  long hour = time.substring(0, 2).toInt();
  long minute = time.substring(3, 5).toInt();
  long second = time.substring(6, 8).toInt();

  DateTime dt(year, month, day, hour, minute, second);
  return dt;
}

#endif //SG_MCU_RTCUTIL_H
