#include "continuousControl.h"

#ifndef SG_MCU_CONTINUOUS_CONTROL_FACTORY_H
#define SG_MCU_CONTINUOUS_CONTROL_FACTORY_H

class ContinuousControlFactory {
public:
  static ContinuousControl *getControl(CONTINUOUS_CONTROL_TYPE_ENUM controlTypeEnum, int channel, void (*dWrite)(int channel, int value)) {

  }
};

#endif //SG_MCU_CONTINUOUS_CONTROL_FACTORY_H
