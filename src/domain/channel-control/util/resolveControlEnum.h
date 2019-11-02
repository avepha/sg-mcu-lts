#include <Arduino.h>
#include "domain/channel-control/control.h"
#ifndef SG_MCU_RESOLVE_CONTROL_TO_ENUM_H
#define SG_MCU_RESOLVE_CONTROL_TO_ENUM_H

String ControlEnumToString(CONTROL_TYPE_ENUM ctrlEnum) {
  switch (ctrlEnum) {
    case CTRL_MANUAL:
      return "manual";
    case CTRL_TIMER:
      return "timer";
    case CTRL_CRITERIA:
      return "criteria";
    default:
      return "none";
  }
}

CONTROL_TYPE_ENUM ControlStringToEnum(String ctrlString) {
  if (ctrlString == "manual") {
    return CTRL_MANUAL;
  }
  else if (ctrlString == "timer") {
    return CTRL_TIMER;
  }
  else if (ctrlString == "criteria") {
    return CTRL_CRITERIA;
  }
  else if (ctrlString == "none"){
    return CTRL_NONE;
  }
  else {
    return CTRL_UNKNOWN;
  }
}

#endif
