#include "TaskScheduler.h"
#include "../control.h"
#include "./model.h"

#ifndef SG_MCU_CTRL_TIMER_CORE_H
#define SG_MCU_CTRL_TIMER_CORE_H

class TimerCore : public Control {
public:
  explicit TimerCore(int channel, void (*dWrite)(int channel, int value)): Control(channel) {
    TimerModel model;
    TimerSchema timerSchema = model.get();
    timer = timerSchema.timers[channel];
  }

  bool Callback() override {
    Serial.println("Timer is running at " + String(channel));
    return true;
  }

private:
  TimerSchema::timer timer;
};


#endif //SG_MCU_CORE_H
