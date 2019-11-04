#include <Arduino.h>
#include "../precondition.h"

#ifndef SG_MCU_RESOLVE_PRECONDITION_ENUM_H
#define SG_MCU_RESOLVE_PRECONDITION_ENUM_H

String PreconditionEnumToString(PRECONDITION_TYPE_ENUM precEnum) {
  if (precEnum == PREC_NONE) {
    return "none";
  }
  else if (precEnum == PREC_TIMER) {
    return "timer";
  }
  else if (precEnum == PREC_CRITERIA) {
    return "criteria";
  }
  else if (precEnum == PREC_RANGE) {
    return "range";
  }
  else {
    return "none";
  }
}

PRECONDITION_TYPE_ENUM PreconditionStringToEnum(String precString) {
  if (precString == "timer") {
    return PREC_TIMER;
  }
  else if (precString == "criteria") {
    return PREC_CRITERIA;
  }
  else if (precString == "range") {
    return PREC_RANGE;
  }
  else if (precString == "none") {
    return PREC_NONE;
  }
  else {
    return PREC_UNKNOWN;
  }
}

#endif // SG_MCU_RESOLVEPRECONDITIONENUM_H
