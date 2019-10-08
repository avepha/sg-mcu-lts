#include <TaskScheduler.h>

#ifndef SG_MCU_CONTROL_H
#define SG_MCU_CONTROL_H
Scheduler ctrlScheduler;

enum CONTROL_TYPE_ENUM {
  CTRL_NONE = 0,
  CTRL_MANUAL,
  CTRL_TIMER,
  CTRL_CRITERIA,
  CTRL_RANGE,
};

class Control: public Task {
public:
  int channel = -1;
  Control(int channel, void (*dWrite)(int, int), int interval = TASK_SECOND): Task(interval, TASK_FOREVER, &ctrlScheduler, true),
      channel(channel),
      dWrite(dWrite){}

  virtual ~Control() = default;;

  bool Callback();

  void (*dWrite)(int, int);
};

bool Control::Callback() {
  return false;
}

#endif //SG_MCU_CONTROL_H
