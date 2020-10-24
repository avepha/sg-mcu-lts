#include <Arduino.h>
#include "../station.h"
#ifndef SG_MCU_RESOLVESTATIONTYPE_H
#define SG_MCU_RESOLVESTATIONTYPE_H

String StationTypeEnumToString(STATION_ENUM type) {
  switch (type) {
    case STATION_GSENSOR:
      return "gsensor";
    case STATION_SOLUTION:
      return "solution";
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
  else {
    return STATION_NULL;
  }
}


#endif //SG_MCU_RESOLVESTATIONTYPE_H
