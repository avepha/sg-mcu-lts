#include <Arduino.h>
#include "../station.h"
#ifndef SG_MCU_RESOLVEPSTATIONTYPE_H
#define SG_MCU_RESOLVEPSTATIONTYPE_H

String PStationTypeEnumToString(PSTATION_ENUM type) {
  switch (type) {
    case PSTATION_GSENSOR:
      return "gsensor";
    case PSTATION_SOLUTION:
      return "solution";
    default:
      return "null";
  }
}

PSTATION_ENUM PStationTypeStringToEnum(const String& strType) {
  if (strType == "gsensor") {
    return PSTATION_GSENSOR;
  }
  else if (strType == "solution") {
    return PSTATION_SOLUTION;
  }
  else {
    return PSTATION_NULL;
  }
}


#endif //SG_MCU_RESOLVESTATIONTYPE_H
