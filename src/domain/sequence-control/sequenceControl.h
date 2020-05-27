#include <TaskScheduler.h>
#include "validationError.h"
#include "sequenceControlTypeEnum.h"
#include "util/resolveSequenceControlEnum.h"
#include "domain/precondition/precondition.h"
#include "domain/sequence/util/sequenceGpioChain.h"

#ifndef SG_MCU_SEQUENCE_CONTROL_H
#define SG_MCU_SEQUENCE_CONTROL_H

class SequenceControl : public Task {
public:
  SequenceControl(SEQUENCE_CONTROL_TYPE_ENUM type, SequenceGpioChain *gpioChain, int interval = TASK_SECOND) : Task(
      interval, TASK_FOREVER, &controlScheduler, false),
                                                                                                               gpioChain(
                                                                                                                   gpioChain),
                                                                                                               type(
                                                                                                                   type) {
    taskName = (SequenceControlEnumToString(type)).c_str();
    gpioCore = GpioCore::instance();
  }

  virtual ~SequenceControl() {
    for (int i = 0; i < precSize; i++) {
      delete preconditions[i];
    }
  };

  virtual bool controlTask(bool isPrecPass) {
    return true;
  };

  bool Callback() override {
    if (deactivationFlag) {
      delete this;
      return true;
    }

    bool isPrecPass = true;
    for (int i = 0; i < precSize; i++) {
      if (!preconditions[i]->resolve()) {
        isPrecPass = false;
      }
    }

    return controlTask(isPrecPass);
  };

  SEQUENCE_CONTROL_TYPE_ENUM getType() {
    return type;
  }

  void addPrecondition(Precondition *_precondition) {
    if (precSize > 2) {
      PrecondtionExceedError err;
      throw err;
    }

    preconditions[precSize] = _precondition;
    precSize++;
  }

  int getPreconditionSize() {
    return precSize;
  }

  Precondition *getPreconditionAt(int index) {
    return preconditions[index];
  }

  // to avoid race condition, between 2 processors
  // so deactivation process must be execute by main core
  // this is why setDeactivation method is in action
  void setDeactivationFlag() {
    deactivationFlag = true;
  }

protected:
  SequenceGpioChain *gpioChain = nullptr;
  GpioCore *gpioCore = nullptr;
  std::string taskName;

private:
  bool deactivationFlag = false;
  SEQUENCE_CONTROL_TYPE_ENUM type;
  uint8_t precSize = 0;
  Precondition *preconditions[3];
};

#endif //SG_MCU_SEQUENCE_CONTROL_H
