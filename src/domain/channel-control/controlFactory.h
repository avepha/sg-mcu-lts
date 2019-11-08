#include "control.h"

#include "./timer/core.h"
#include "./range/core.h"
#include "./criteria/core.h"
#include "./par/core.h"

#ifndef SG_MCU_CONTROL_FACTORY_H
#define SG_MCU_CONTROL_FACTORY_H

class ControlFactory {
public:
  static Control *getControl(CONTROL_TYPE_ENUM controlTypeEnum, int channel) {
    switch (controlTypeEnum) {
      case CH_CTRL_TIMER:
        return new TimerCore(channel);
      case CH_CTRL_CRITERIA:
        return new CriteriaCore(channel);
      case CH_CTRL_RANGE:
        return new RangeCore(channel);
      case CH_CTRL_PAR:
        return new ParCore(channel);
      default:
        return nullptr;
    }
  }
};

#endif //SG_MCU_CONTROL_FACTORY_H
