#include "./precondition.h"
#include "./timer/core.h"
#include "./criteria/core.h"
#include "./range/core.h"

#ifndef SG_MCU_PRECONDITION_FACTORY_H
#define SG_MCU_PRECONDITION_FACTORY_H

class PreconditionFactory {
public:
  static Precondition *getPrecondition(PRECONDITION_TYPE_ENUM preconditionTypeEnum, int channel) {
    switch (preconditionTypeEnum) {
      case PREC_TIMER:
        return new PrecTimerCore(channel);
      case PREC_CRITERIA:
        return new PrecCriteriaCore(channel);
      case PREC_RANGE:
        return new PrecRangeCore(channel);
      default:
        return nullptr;
    }
  }
};

#endif //SG_MCU_PRECONDITION_FACTORY_H
