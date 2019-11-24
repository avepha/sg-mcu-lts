#include <Arduino.h>
#include "domain/channel-control/controlTypeEnum.h"

#ifndef SG_MCU_RESOLVE_CHANNEL_CONTROL_TO_ENUM_H
#define SG_MCU_RESOLVE_CHANNEL_CONTROL_TO_ENUM_H

String ChannelControlEnumToString(CONTROL_TYPE_ENUM ctrlEnum) {
  switch (ctrlEnum) {
    case CH_CTRL_MANUAL:
      return "manual";
    case CH_CTRL_TIMER:
      return "timer";
    case CH_CTRL_CRITERIA:
      return "criteria";
    case CH_CTRL_RANGE:
      return "range";
    case CH_CTRL_PAR:
      return "par";
    default:
      return "none";
  }
}

String ChannelControlEnumToShortString(CONTROL_TYPE_ENUM ctrlEnum) {
  switch (ctrlEnum) {
    case CH_CTRL_MANUAL:
      return "CHMNL";
    case CH_CTRL_TIMER:
      return "CHTMR";
    case CH_CTRL_CRITERIA:
      return "CMCRT";
    case CH_CTRL_RANGE:
      return "CHRNG";
    case CH_CTRL_PAR:
      return "CHPAR";
    default:
      return "NONE";
  }
}

CONTROL_TYPE_ENUM ChannelControlStringToEnum(const String& ctrlString) {
  if (ctrlString == "manual") {
    return CH_CTRL_MANUAL;
  }
  else if (ctrlString == "timer") {
    return CH_CTRL_TIMER;
  }
  else if (ctrlString == "criteria") {
    return CH_CTRL_CRITERIA;
  }
  else if (ctrlString == "range") {
    return CH_CTRL_RANGE;
  }
  else if (ctrlString == "par"){
    return CH_CTRL_PAR;
  }
  else if (ctrlString == "none"){
    return CH_CTRL_NONE;
  }
  else {
    return CH_CTRL_UNKNOWN;
  }
}

#endif //SG_MCU_RESOLVE_CHANNEL_CONTROL_TO_ENUM_H
