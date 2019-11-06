#include "domain/continuous-control/continuousControl.h"
#include "domain/continuous-control/continuousControlFactory.h"
#include "./model.h"
#include "./util/continuousGpioChain.h"

#ifndef SG_MCU_CONTINUOUS_CORE_H
#define SG_MCU_CONTINUOUS_CORE_H

class ContinuousCore {
public:
  static ContinuousCore *instance() {
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
    deactivateControls();

    ContinuousModel model;
    ContinuousSchema::Continuous continuous = model.get().continuous;

    if (!continuous.isActive) {
      gpioCore->removeGpioTaskAll();
      return;
    }

    gpioChain = new ContinuousGpioChain();
    for (int i = 0; i < continuous.control.channelOrderAndTimingSize; i++) {
      gpioChain->add(
          continuous.control.channelOrderAndTiming[i].channel,
          continuous.control.channelOrderAndTiming[i].workingTimeInSec
      );
    }

    switch (continuous.control.type) {
      case CON_CTRL_TIMER:
        break;
      case CON_CTRL_CRITERIA:
        ContinuousControlFactory::getControl(CON_CTRL_CRITERIA, gpioChain);
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

    if (gpioChain != nullptr) {
      gpioChain->disable();
      delete gpioChain;
      gpioChain = nullptr;
    }
  }

private:
  static ContinuousCore *s_instance;
  ContinuousControl *continuousControl = nullptr;
  GpioCore *gpioCore;
  ContinuousGpioChain *gpioChain = nullptr;
};

ContinuousCore *ContinuousCore::s_instance = nullptr;

#endif //SG_MCU_CONTINUOUS_CORE_H
