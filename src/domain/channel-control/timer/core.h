
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
  ChannelTimerModeEnum mode;
  uint32_t nextIntervalTimerInSeconds[2] = {0};
  uint32_t currentIntervalTimerInSeconds[2] = {0};

  uint32_t currentWorkingTimerInSeconds[2] = {0};
  uint32_t nextWorkingTimerInSeconds[2] = {0};

  DynamicJsonDocument report() override {
    DynamicJsonDocument data(256);
    data["type"] = "timer";
    data["currentTimeInSecond"] = currentTimeInSecond;
    data["isReachThreshold"] = isReachThreshold;
    data["mode"] = ChannelTimerModeEnumToString(mode);
    switch(mode) {
      case CH_TIMER_INTERVAL: {
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
      }
      case CH_TIMER_WORKING: {
        if (isReachThreshold) {
          JsonObject _currentTime = data.createNestedObject("currentWorkingTimerInSeconds");
          _currentTime["start"] = currentWorkingTimerInSeconds[0];
          _currentTime["working"] = currentWorkingTimerInSeconds[1];
        }
        else {
          JsonObject _currentTime = data.createNestedObject("nextWorkingTimerInSeconds");
          _currentTime["start"] = nextWorkingTimerInSeconds[0];
          _currentTime["working"] = nextWorkingTimerInSeconds[1];
        }
      }
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
    state.mode = timerSchema.timers[channel].mode;
    rtcCore = RtcCore::instance();
  }

  TimerState getControlState() {
    return state;
  }

  bool controlTask(bool _isPrecPass) override {
    DateTime dt = rtcCore->getDate();
    state.currentTimeInSecond = dt.hour() * 3600 + dt.minute() * 60 + dt.second();

    switch (state.mode) {
      case CH_TIMER_INTERVAL: {
        bool flag = false;
        for (int i = 0; i < timer.size; i++) {
          state.isReachThreshold = state.currentTimeInSecond >= timer.timePair[i].start * 60 &&
                                   state.currentTimeInSecond <= timer.timePair[i].stop * 60;
          if (state.isReachThreshold) {
            flag = true;
            state.currentIntervalTimerInSeconds[0] = timer.timePair[i].start * 60; // start time
            state.currentIntervalTimerInSeconds[1] = timer.timePair[i].stop * 60; // stop time
          }

          // if isReachThreshold = false, store next coming timer in state.nextIntervalTimerInSeconds
          if (state.currentTimeInSecond < timer.timePair[i].start * 60) {
            state.nextIntervalTimerInSeconds[0] = timer.timePair[i].start * 60; // start time
            state.nextIntervalTimerInSeconds[1] = timer.timePair[i].stop * 60; // stop time
          }
        }

        if (flag) {
          gpioTask->high();
        }
        else {
          gpioTask->low();
        }
        break;
      }
      case CH_TIMER_WORKING: {
        bool flag = false;
        uint32_t workingTimeout = 0;
        for (int i = 0; i < timer.size; i++) {
          uint32_t startTimeInSeconds = timer.timePair[i].start * 60;
          state.isReachThreshold = state.currentTimeInSecond >= startTimeInSeconds &&
                                   state.currentTimeInSecond <= startTimeInSeconds + timer.timePair[i].workingInSecond;

          if (state.isReachThreshold) {
            flag = true;
            state.currentWorkingTimerInSeconds[0] = timer.timePair[i].start * 60; // start time
            state.currentWorkingTimerInSeconds[1] = timer.timePair[i].workingInSecond;
          }

          // if isReachThreshold = false, store next coming timer in state.nextIntervalTimerInSeconds
          if (state.currentTimeInSecond < timer.timePair[i].start * 60) {
            state.nextWorkingTimerInSeconds[0] = timer.timePair[i].start * 60; // start time
            state.nextWorkingTimerInSeconds[1] = timer.timePair[i].workingInSecond;
          }
        }

        if (flag) {
          gpioTask->high();
        }
        else {
          gpioTask->low();
        }
      }
    }
    return true;
  }

private:
  TimerState state;
  TimerSchema::Timer timer;
  RtcCore *rtcCore;
};


#endif //SG_MCU_CORE_H
