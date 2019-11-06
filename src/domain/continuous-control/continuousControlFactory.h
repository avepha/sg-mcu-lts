#include "continuousControl.h"
#include "domain/channel-control/control.h"
#include "domain/channel-control/criteria/core.h"
#include "./continuous-criteria/core.h"
#include "domain/continuous/util/continuousGpioChain.h"

#ifndef SG_MCU_CONTINUOUS_CONTROL_FACTORY_H
#define SG_MCU_CONTINUOUS_CONTROL_FACTORY_H

class ContinuousControlFactory {
public:
  static ContinuousControl* getControl(CONTINUOUS_CONTROL_TYPE_ENUM controlTypeEnum, ContinuousGpioChain *gpioChain) {
    switch (controlTypeEnum) {
      case CON_CTRL_CRITERIA:
        return new ContinuousCriteriaCore(gpioChain);
      default:
        return nullptr;
    }
  }
};

#endif //SG_MCU_CONTINUOUS_CONTROL_FACTORY_H
