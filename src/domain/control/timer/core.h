#include "TaskScheduler.h"
#include "domain/rtc/core.h"
#include "../control.h"
#include "./model.h"

#ifndef SG_MCU_CTRL_TIMER_CORE_H
#define SG_MCU_CTRL_TIMER_CORE_H

class TimerCore : public Control {
public:
  explicit TimerCore(int channel, void (*dWrite)(int channel, int value)): Control(channel, dWrite) {
    TimerModel model;
    TimerSchema timerSchema = model.get();
    timer = timerSchema.timers[channel];
    rtcCore = new RtcCore;
  }

  ~TimerCore() override {
    delete rtcCore;
  }

  bool Callback() override {
    DateTime dt = rtcCore->getDate();
    long currentInMinute = dt.hour() * 60 + dt.minute();

    for (int i = 0; i < timer.size; i++)
    {
      if (currentInMinute >= timer.data[i][0] && currentInMinute <= timer.data[i][1])
      {
        dWrite(channel, HIGH);
        break;
      }
      dWrite(channel, LOW);
    }
    return true;
  }


private:
  TimerSchema::timer timer;
  RtcCore *rtcCore;
};


#endif //SG_MCU_CORE_H
