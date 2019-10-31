#include "./channel-control/control.h"

#include "./channel-control/timer/core.h"
#include "./channel-control/range/core.h"
#include "./channel-control/criteria/core.h"

#ifndef SG_MCU_CONTROL_FACTORY_H
#define SG_MCU_CONTROL_FACTORY_H

class ControlFactory {
public:
  static Control *getControl(CONTROL_TYPE_ENUM controlTypeEnum, int channel, void (*dWrite)(int channel, int value)) {
    switch (controlTypeEnum) {
      case CTRL_TIMER:
        return new TimerCore(channel, dWrite);
      case CTRL_CRITERIA:
        return new CriteriaCore(channel, dWrite);
      case CTRL_RANGE:
        return new RangeCore(channel, dWrite);
      default:
        return nullptr;
    }
  }
};

#endif //SG_MCU_CONTROL_FACTORY_H
