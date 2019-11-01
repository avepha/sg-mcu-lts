#include <Arduino.h>
#include "domain/continuous-control/continuousControlTypeEnum.h"
#ifndef SG_MCU_RESOLVE_CONTINUOUS_CONTROL_TO_ENUM_H
#define SG_MCU_RESOLVE_CONTINUOUS_CONTROL_TO_ENUM_H

String ContinuousControlEnumToString(CONTINUOUS_CONTROL_TYPE_ENUM ctrlEnum) {
  switch (ctrlEnum) {
    case CON_CTRL_TIMER:
      return "continuous-timer";
    case CON_CTRL_CRITERIA:
      return "continuous-criteria";
    default:
      return "none";
  }
}

CONTINUOUS_CONTROL_TYPE_ENUM ContinuousControlStringToEnum(const String& ctrlString) {
  if (ctrlString == "continuous-timer") {
    return CON_CTRL_TIMER;
  }
  else if (ctrlString == "continuous-criteria") {
    return CON_CTRL_CRITERIA;
  }
  else {
    return CON_CTRL_NONE;
  }
}

#endif
