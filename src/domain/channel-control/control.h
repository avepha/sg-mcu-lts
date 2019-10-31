#include <TaskScheduler.h>
#include "validationError.h"
#include "domain/channel-control/state.h"
#include "domain/precondition/precondition.h"

#ifndef SG_MCU_CONTROL_H
#define SG_MCU_CONTROL_H
Scheduler ctrlScheduler;

enum CONTROL_TYPE_ENUM {
  CTRL_NONE = 0,
  CTRL_MANUAL = 1,
  CTRL_TIMER = 2,
  CTRL_CRITERIA = 3,
  CTRL_RANGE = 4,
};

class Control: public Task {
public:
  int channel = -1;

  Control(int channel, CONTROL_TYPE_ENUM type, void (*dWrite)(int, int), int interval = TASK_SECOND): Task(interval, TASK_FOREVER, &ctrlScheduler, false),
      channel(channel),
      dWrite(dWrite),
      type(type) {}

  virtual ~Control() {
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

  CONTROL_TYPE_ENUM getType() {
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

  Precondition* getPreconditionAt(int index) {
    return preconditions[index];
  }

private:
  CONTROL_TYPE_ENUM type;
  uint8_t precSize = 0;
  Precondition *preconditions[3];
};

#endif //SG_MCU_CONTROL_H
