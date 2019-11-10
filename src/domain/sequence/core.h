#include "domain/sequence-control/sequenceControl.h"
#include "domain/sequence-control/sequenceControlFactory.h"
#include "./model.h"
#include "./util/sequenceGpioChain.h"

#ifndef SG_MCU_SEQUENCE_CORE_H
#define SG_MCU_SEQUENCE_CORE_H

class SequenceCore {
public:
  static SequenceCore *instance() {
    if (!s_instance)
      s_instance = new SequenceCore;
    return s_instance;
  }

  SequenceControl *getControl() {
    return sequenceControl;
  }

  void activateControls() {
    deactivateControls();

    SequenceModel model;
    SequenceSchema::Sequence sequence = model.get().sequence;

    if (!sequence.isActive) {
      gpioCore->removeGpioTaskAll();
      return;
    }

    gpioChain = new SequenceGpioChain();
    for (int i = 0; i < sequence.control.channelOrderAndTimingSize; i++) {
      gpioChain->add(
          sequence.control.channelOrderAndTiming[i].channel,
          sequence.control.channelOrderAndTiming[i].workingTimeInSec
      );
    }

    SequenceControl *control = SequenceControlFactory::getControl(sequence.control.type, gpioChain);
    if (control != nullptr) {
      sequenceControl = control;

      for (int i = 0; i < sizeof(sequence.preconditions) / sizeof(sequence.preconditions[0]); i++) {
        Precondition *precondition = PreconditionFactory::getPrecondition(sequence.preconditions[i].type, 1);
        if (precondition != nullptr) {
          sequenceControl->addPrecondition(precondition);
        }
      }
      sequenceControl->enable();
    }
    else {
      gpioCore->removeGpioTaskAll();
    }
  }

  void deactivateControls() {
    if (sequenceControl != nullptr) {
      sequenceControl->disable();
      delete sequenceControl;
      sequenceControl = nullptr;
    }

    if (gpioChain != nullptr) {
      gpioChain->disable();
      delete gpioChain;
      gpioChain = nullptr;
    }
  }

private:
  SequenceCore() {
    gpioCore = GpioCore::instance();
    for (int i = 0; i < sizeof(CHANNEL_GPIO_MAP) / sizeof(CHANNEL_GPIO_MAP[0]); i++) {
      pinMode(CHANNEL_GPIO_MAP[i], OUTPUT);
    }
  }

  static SequenceCore *s_instance;
  SequenceControl *sequenceControl = nullptr;
  GpioCore *gpioCore;
  SequenceGpioChain *gpioChain = nullptr;
};

SequenceCore *SequenceCore::s_instance = 0;

#endif //SG_MCU_SEQUENCE_CORE_H