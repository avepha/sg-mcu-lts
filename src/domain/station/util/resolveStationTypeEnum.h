#include <Arduino.h>
#include "domain/station/station.h"
#ifndef SG_MCU_RESOLVESTATIONTYPE_H
#define SG_MCU_RESOLVESTATIONTYPE_H

String StationTypeEnumToString(STATION_ENUM type) {
  switch (type) {
    case STATION_GSENSOR:
      return "gsensor";
    case STATION_SOLUTION:
      return "solution";
    case STATION_GSOLUTION:
      return "gsolution";
    default:
      return "null";
  }
}

STATION_ENUM StationTypeStringToEnum(const String& strType) {
  if (strType == "gsensor") {
    return STATION_GSENSOR;
  }
  else if (strType == "solution") {
    return STATION_SOLUTION;
  }
  else if (strType == "gsolution") {
    return STATION_GSOLUTION;
  }
  else {
    return static_cast<STATION_ENUM>(-1);
  }
}


#endif //SG_MCU_RESOLVESTATIONTYPE_H
