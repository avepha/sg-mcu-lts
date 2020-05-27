
#include "TaskScheduler.h"
#include "domain/rtc/core.h"
#include "domain/channel-control/state.h"
#include "domain/channel-control/control.h"
#include "./model.h"

#ifndef SG_MCU_CTRL_TIMER_CORE_H
#define SG_MCU_CTRL_TIMER_CORE_H

class TimerState : public State {
public:
  unsigned long currentTimeInSecond = 0;
  bool isReachThreshold = false;
  uint32_t nextIntervalTimerInSeconds[2] = {0};
  uint32_t currentIntervalTimerInSeconds[2] = {0};

  DynamicJsonDocument report() override {
    DynamicJsonDocument data(256);
    data["type"] = "timer";
    data["currentTimeInSecond"] = currentTimeInSecond;
    data["isReachThreshold"] =  isReachThreshold;
    if (isReachThreshold) {
      JsonObject _currentTime = data.createNestedObject("currentIntervalTimerInSeconds");
      _currentTime["start"] = currentIntervalTimerInSeconds[0];
      _currentTime["stop"] = currentIntervalTimerInSeconds[1];
    }
    else {
      JsonObject _currentTime = data.createNestedObject("nextIntervalTimerInSeconds");
      _currentTime["start"] = nextIntervalTimerInSeconds[0];
      _currentTime["stop"] = nextIntervalTimerInSeconds[1];
    }

    return data;
  }
};

class TimerCore : public Control {
public:
  explicit TimerCore(int channel) : Control(channel, CH_CTRL_TIMER) {
    TimerModel model;
    TimerSchema timerSchema = model.get();
    timer = timerSchema.timers[channel];
    rtcCore = RtcCore::instance();
  }

  TimerState getControlState() {
    return state;
  }

  bool controlTask(bool _isPrecPass) override {
    DateTime dt = rtcCore->getDate();
    state.currentTimeInSecond = dt.hour() * 3600 + dt.minute() * 60 + dt.second();

    for (int i = 0; i < timer.size; i++) {
      state.isReachThreshold = state.currentTimeInSecond >= timer.timePair[i].start * 60 && state.currentTimeInSecond <= timer.timePair[i].stop * 60;
      if (state.isReachThreshold) {
        state.currentIntervalTimerInSeconds[0] = timer.timePair[i].start * 60; // start time
        state.currentIntervalTimerInSeconds[1] = timer.timePair[i].stop * 60; // stop time

        gpioCore->createGpioTaskForever(taskName, channel);
        break;
      }

      // if isReachThreshold = false, store next coming timer in state.nextIntervalTimerInSeconds
      if (state.currentTimeInSecond < timer.timePair[i].start * 60) {
        state.nextIntervalTimerInSeconds[0] = timer.timePair[i].start * 60; // start time
        state.nextIntervalTimerInSeconds[1] = timer.timePair[i].stop * 60; // stop time
      }

      gpioCore->removeGpioTaskByChannel(channel);
    }
    return true;
  }

private:
  TimerState state;
  TimerSchema::Timer timer;
  RtcCore *rtcCore;
};


#endif //SG_MCU_CORE_H
