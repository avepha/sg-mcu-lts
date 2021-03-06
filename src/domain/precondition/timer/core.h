#include "../precondition.h"
#include "domain/channel-control/state.h"
#include "domain/rtc/core.h"
#include "./model.h"

#ifndef SG_MCU_PRECONDITION_TIMER_CORE_H
#define SG_MCU_PRECONDITION_TIMER_CORE_H

class PrecTimerState : public State {
public:
  unsigned long currentTimeInSecond = 0;
  bool isReachThreshold = false;
  uint32_t nextIntervalTimerInSeconds[2] = {0};
  uint32_t currentIntervalTimerInSeconds[2] = {0};

  DynamicJsonDocument report() override {
    DynamicJsonDocument data(256);
    data["type"] = "precTimer";
    data["currentTimeInSecond"] = currentTimeInSecond;
    data["isReachThreshold"] = isReachThreshold;
    if (isReachThreshold) {
      JsonObject _currentTime = data.createNestedObject("currentIntervalTimerInSeconds");
      _currentTime["start"] = currentIntervalTimerInSeconds[0];
      _currentTime["stop"] = currentIntervalTimerInSeconds[1];
    } else {
      JsonObject _currentTime = data.createNestedObject("nextIntervalTimerInSeconds");
      _currentTime["start"] = nextIntervalTimerInSeconds[0];
      _currentTime["stop"] = nextIntervalTimerInSeconds[1];
    }

    return data;
  }
};

class PrecTimerCore : public Precondition {
public:
  explicit PrecTimerCore(int channel) : Precondition(PREC_TIMER) {
    PrecTimerModel model;
    PrecTimerSchema timerSchema = model.get();
    timer = timerSchema.timers[channel];
    rtcCore = RtcCore::instance();
  }

  bool resolve() override {
    DateTime dt = rtcCore->getDate();
    state.currentTimeInSecond = dt.hour() * 3600 + dt.minute() * 60 + dt.second();

    for (int i = 0; i < timer.size; i++) {
      // if isReachThreshold = false, store next coming timer in state.nextIntervalTimerInSeconds
      if (state.currentTimeInSecond < timer.timePair[i].start * 60) {
        state.nextIntervalTimerInSeconds[0] = timer.timePair[i].start * 60; // start time
        state.nextIntervalTimerInSeconds[1] = timer.timePair[i].stop * 60; // stop time
      }

      state.isReachThreshold =
          state.currentTimeInSecond >= timer.timePair[i].start * 60 && state.currentTimeInSecond <= timer.timePair[i].stop * 60;
      if (state.isReachThreshold) {
        state.currentIntervalTimerInSeconds[0] = timer.timePair[i].start * 60; // start time
        state.currentIntervalTimerInSeconds[1] = timer.timePair[i].stop * 60; // stop time
        return true;
      }
    }
    return false;
  }

  PrecTimerState getPreconditionState() {
    return state;
  }


private:
  PrecTimerState state;
  PrecTimerSchema::Timer timer;
  RtcCore *rtcCore{};
};

#endif //SG_MCU_CORE_H
