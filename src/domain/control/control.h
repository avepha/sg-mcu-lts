#include <TaskScheduler.h>

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
  Control(int channel, CONTROL_TYPE_ENUM type, void (*dWrite)(int, int), int interval = TASK_SECOND): Task(interval, TASK_FOREVER, &ctrlScheduler, true),
      channel(channel),
      dWrite(dWrite),
      type(type) {}

  virtual ~Control() = default;;

  bool Callback();

  void (*dWrite)(int, int);

  CONTROL_TYPE_ENUM getType() {
    return type;
  }

private:
  CONTROL_TYPE_ENUM type;
};

bool Control::Callback() {
  return false;
}

#endif //SG_MCU_CONTROL_H
