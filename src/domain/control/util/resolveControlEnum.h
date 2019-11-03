#include <Arduino.h>
#include "../controlEnum.h"

#ifndef SG_MCU_RESOLVE_CONTROL_TO_ENUM_H
#define SG_MCU_RESOLVE_CONTROL_TO_ENUM_H

String ControlEnumToString(CONTROL_ENUM ctrlEnum) {
  switch (ctrlEnum) {
    case CTRL_CHANNEL:
      return "channel";
    case CTRL_CONTINUOUS:
      return "continuous";
    default:
      return "none";
  }
}

CONTROL_ENUM ControlStringToEnum(const String& ctrlString) {
  if (ctrlString == "channel") {
    return CTRL_CHANNEL;
  }
  else if (ctrlString == "continuous") {
    return CTRL_CONTINUOUS;
  }
  else if (ctrlString == "none"){
    return CTRL_NONE;
  }
  else {
    return CTRL_UNKNOWN;
  }
}

#endif //SG_MCU_RESOLVE_CONTROL_CONTROL_TO_ENUM_H
