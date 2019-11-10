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
  SequenceControl(SEQUENCE_CONTROL_TYPE_ENUM type, SequenceGpioChain *gpioChain,int interval = TASK_SECOND) : Task(interval, TASK_FOREVER, &controlScheduler, false),
    gpioChain(gpioChain),
    type(type)
    {
      taskName = (SequenceControlEnumToString(type)).c_str();
      gpioCore = GpioCore::instance();
    }

  virtual ~SequenceControl() {
    for (int i = 0; i < precSize; i++) {
      delete preconditions[i];
    }
  };

  virtual bool controlTask() {
    return true;
  };

  bool Callback() override {
    for (int i = 0; i < precSize; i++) {
      if (!preconditions[i]->resolve()) {
        return false;
      }
    }

    return controlTask();
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

protected:
  SequenceGpioChain *gpioChain = nullptr;
  GpioCore *gpioCore = nullptr;
  std::string taskName;

private:
  SEQUENCE_CONTROL_TYPE_ENUM type;
  uint8_t precSize = 0;
  Precondition *preconditions[3];
};

#endif //SG_MCU_SEQUENCE_CONTROL_H