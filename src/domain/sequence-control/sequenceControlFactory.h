#include "sequenceControl.h"
#include "domain/channel-control/control.h"
#include "./sequence-criteria/core.h"

#include "domain/sequence-control/sequence-criteria/core.h"
#include "domain/sequence-control/sequence-timer/core.h"
#include "domain/sequence-control/sequence-range/core.h"

#include "domain/sequence/util/sequenceGpioChain.h"

#ifndef SG_MCU_SEQUENCE_CONTROL_FACTORY_H
#define SG_MCU_SEQUENCE_CONTROL_FACTORY_H

class SequenceControlFactory {
public:
  static SequenceControl* getControl(SEQUENCE_CONTROL_TYPE_ENUM controlTypeEnum, SequenceGpioChain *gpioChain) {
    switch (controlTypeEnum) {
      case SEQ_CTRL_CRITERIA:
        return new SequenceCriteriaCore(gpioChain);
      case SEQ_CTRL_TIMER:
        return new SequenceTimerCore(gpioChain);
      case SEQ_CTRL_RANGE:
        return new SequenceRangeCore(gpioChain);
      default:
        return nullptr;
    }
  }
};

#endif //SG_MCU_SEQUENCE_CONTROL_FACTORY_H
