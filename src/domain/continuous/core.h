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

  ContinuousControl *getControl() {
    return continuousControl;
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

    ContinuousControl *control = ContinuousControlFactory::getControl(CON_CTRL_CRITERIA, gpioChain);
    if (control != nullptr) {
      continuousControl = control;

      for (int i = 0; i < sizeof(continuous.preconditions) / sizeof(continuous.preconditions[0]); i++) {
        Precondition *precondition = PreconditionFactory::getPrecondition(continuous.preconditions[i].type, 1);
        if (precondition != nullptr) {
          continuousControl->addPrecondition(precondition);
        }
      }
      continuousControl->enable();
    }
    else {
      gpioCore->removeGpioTaskAll();
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
  ContinuousCore() {
    gpioCore = GpioCore::instance();
    for (int i = 0; i < sizeof(CHANNEL_GPIO_MAP) / sizeof(CHANNEL_GPIO_MAP[0]); i++) {
      pinMode(CHANNEL_GPIO_MAP[i], OUTPUT);
    }
  }

  static ContinuousCore *s_instance;
  ContinuousControl *continuousControl = nullptr;
  GpioCore *gpioCore;
  ContinuousGpioChain *gpioChain = nullptr;
};

ContinuousCore *ContinuousCore::s_instance = 0;

#endif //SG_MCU_CONTINUOUS_CORE_H
