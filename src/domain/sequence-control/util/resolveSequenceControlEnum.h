#include <Arduino.h>
#include "domain/sequence-control/sequenceControlTypeEnum.h"
#ifndef SG_MCU_RESOLVE_SEQUENCE_CONTROL_TO_ENUM_H
#define SG_MCU_RESOLVE_SEQUENCE_CONTROL_TO_ENUM_H

String SequenceControlEnumToString(SEQUENCE_CONTROL_TYPE_ENUM ctrlEnum) {
  switch (ctrlEnum) {
    case SEQ_CTRL_TIMER:
      return "sequence-timer";
    case SEQ_CTRL_CRITERIA:
      return "sequence-criteria";
    case SEQ_CTRL_RANGE:
      return "sequence-range";
    default:
      return "none";
  }
}

SEQUENCE_CONTROL_TYPE_ENUM SequenceControlStringToEnum(const String& ctrlString) {
  if (ctrlString == "sequence-timer") {
    return SEQ_CTRL_TIMER;
  }
  else if (ctrlString == "sequence-criteria") {
    return SEQ_CTRL_CRITERIA;
  }
  else if (ctrlString == "sequence-range") {
    return SEQ_CTRL_RANGE;
  }
  else if (ctrlString == "none"){
    return SEQ_CTRL_NONE;
  }
  else {
    return SEQ_CTRL_UNKNOWN;
  }
}

#endif
