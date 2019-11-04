#include <TaskScheduler.h>
#include "validationError.h"
#include "continuousControlTypeEnum.h"
#include "domain/precondition/precondition.h"

#ifndef SG_MCU_CONTINUOUS_CONTROL_H
#define SG_MCU_CONTINUOUS_CONTROL_H

class ContinuousControl : public Task {
public:
  int channel = -1;

  ContinuousControl(int channel,
                    CONTINUOUS_CONTROL_TYPE_ENUM type,
                    void (*dWrite)(int, int),
                    int interval = TASK_SECOND) : Task(interval, TASK_FOREVER, &controlScheduler, false),
                                                  channel(channel),
                                                  dWrite(dWrite),
                                                  type(type) {}

  virtual ~ContinuousControl() {
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

  void (*dWrite)(int, int);

  CONTINUOUS_CONTROL_TYPE_ENUM getType() {
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

private:
  CONTINUOUS_CONTROL_TYPE_ENUM type;
  uint8_t precSize = 0;
  Precondition *preconditions[3];
};

#endif //SG_MCU_CONTINUOUS_CONTROL_H
