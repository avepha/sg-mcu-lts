#include <Arduino.h>
#include "domain/sequence-control/sequenceControlTypeEnum.h"

#ifndef SG_MCU_RESOLVE_SEQUENCE_CONTROL_TO_ENUM_H
#define SG_MCU_RESOLVE_SEQUENCE_CONTROL_TO_ENUM_H

String SequenceControlEnumToString(SEQUENCE_CONTROL_TYPE_ENUM ctrlEnum) {
  switch (ctrlEnum) {
    case SEQ_CTRL_TIMER:
      return "sequence_timer";
    case SEQ_CTRL_CRITERIA:
      return "sequence_criteria";
    case SEQ_CTRL_RANGE:
      return "sequence_range";
    case SEQ_CTRL_PAR:
      return "sequence_par";
    default:
      return "none";
  }
}

String SequenceControlEnumToShortString(SEQUENCE_CONTROL_TYPE_ENUM ctrlEnum) {
  switch (ctrlEnum) {
    case SEQ_CTRL_TIMER:
      return "SEGTMR";
    case SEQ_CTRL_CRITERIA:
      return "SEGCRT";
    case SEQ_CTRL_RANGE:
      return "SEGRNG";
    case SEQ_CTRL_PAR:
      return "SEGPAR";
    default:
      return "NONE";
  }
}

SEQUENCE_CONTROL_TYPE_ENUM SequenceControlStringToEnum(const String &ctrlString) {
  if (ctrlString == "sequence_timer") {
    return SEQ_CTRL_TIMER;
  }
  else if (ctrlString == "sequence_criteria") {
    return SEQ_CTRL_CRITERIA;
  }
  else if (ctrlString == "sequence_range") {
    return SEQ_CTRL_RANGE;
  }
  else if (ctrlString == "sequence_par") {
    return SEQ_CTRL_PAR;
  }
  else if (ctrlString == "none") {
    return SEQ_CTRL_NONE;
  }
  else {
    return SEQ_CTRL_UNKNOWN;
  }
}

#endif
