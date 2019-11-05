#include "domain/continuous-control/continuousControl.h"
#include "./model.h"

#ifndef SG_MCU_CONTINUOUS_CORE_H
#define SG_MCU_CONTINUOUS_CORE_H

class ContinuousCore {
public:
  static ContinuousCore* instance()
  {
    if (!s_instance)
      s_instance = new ContinuousCore;
    return s_instance;
  }

  ContinuousCore() {
    gpioCore = GpioCore::instance();
    for (int i = 0; i < sizeof(CHANNEL_GPIO_MAP) / sizeof(CHANNEL_GPIO_MAP[0]); i++) {
      pinMode(CHANNEL_GPIO_MAP[i], OUTPUT);
    }
  }

  void activateControls() {
    if (continuousControl != nullptr) {
      continuousControl->disable();
      delete continuousControl;
      continuousControl = nullptr;
    }

    ContinuousModel model;
    ContinuousSchema::Continuous continuous = model.get().continuous;

    if (!continuous.isActive) {
      for (int i = 0; i < sizeof(CHANNEL_GPIO_MAP) / sizeof(CHANNEL_GPIO_MAP[0]); i++) {
        gpioCore->removeGpioTaskAll();
      }
      return;
    }

    switch (continuous.control.type) {
      case CON_CTRL_TIMER:
        break;
      case CON_CTRL_CRITERIA:
        break;
      case CON_CTRL_RANGE:
        break;
      default:
        gpioCore->removeGpioTaskAll();
        break;
    }
  }

  void deactivateControls() {
    if (continuousControl != nullptr) {
      continuousControl->disable();
      delete continuousControl;
      continuousControl = nullptr;
    }
  }

private:
  static ContinuousCore *s_instance;
  ContinuousControl *continuousControl = nullptr;
  GpioCore *gpioCore;
};

ContinuousCore *ContinuousCore::s_instance = nullptr;

#endif //SG_MCU_CONTINUOUS_CORE_H
