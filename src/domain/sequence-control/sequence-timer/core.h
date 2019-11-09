#include "TaskScheduler.h"
#include "domain/rtc/core.h"
#include "domain/sequence-control/sequenceState.h"
#include "domain/sequence-control/sequenceControl.h"
#include "domain/sequence/util/sequenceGpioChain.h"
#include "./model.h"

#ifndef SG_MCU_SEQUENCE_TIMER_CORE_H
#define SG_MCU_SEQUENCE_TIMER_CORE_H

class SequenceTimerState : public SequenceState {
public:
  unsigned long currentTimeInSecond = 0;
  bool isReachThreshold = false;
  uint32_t nextIntervalTimerInSeconds[2] = {0};
  uint32_t currentIntervalTimerInSeconds[2] = {0};

  JsonDocument report() override {
    DynamicJsonDocument data(256);
    data["type"] = "timer";
    data["currentTimeInSecond"] = currentTimeInSecond;
    data["isReachThreshold"] = isReachThreshold;
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

class SequenceTimerCore : public SequenceControl {
public:
  explicit SequenceTimerCore(SequenceGpioChain *gpioChain) : SequenceControl(SEQ_CTRL_TIMER, gpioChain) {
    SequenceTimerModel model;
    timer = model.get().timer;
    rtcCore = RtcCore::instance();
  }

  ~SequenceTimerCore() override = default;

  SequenceTimerState getControlState() {
    return state;
  }

  bool controlTask() override {
    DateTime dt = rtcCore->getDate();
    state.currentTimeInSecond = dt.hour() * 3600 + dt.minute() * 60 + dt.second();

    for (int i = 0; i < timer.size; i++) {
      state.isReachThreshold =
          state.currentTimeInSecond >= timer.data[i][0] * 60 && state.currentTimeInSecond <= timer.data[i][1] * 60;
      if (state.isReachThreshold) {
        state.currentIntervalTimerInSeconds[0] = timer.data[i][0] * 60; // start time
        state.currentIntervalTimerInSeconds[1] = timer.data[i][1] * 60; // stop time

        if (!gpioChain->isEnabled())
          gpioChain->enable();
        break;
      }

      // if isReachThreshold = false, store next coming timer in state.nextIntervalTimerInSeconds
      if (state.currentTimeInSecond < timer.data[i][0] * 60) {
        state.nextIntervalTimerInSeconds[0] = timer.data[i][0] * 60; // start time
        state.nextIntervalTimerInSeconds[1] = timer.data[i][1] * 60; // stop time
      }

      if (!gpioChain->isEnabled())
        gpioChain->disable();
    }
    return true;
  }

private:
  RtcCore *rtcCore = nullptr;
  SequenceTimerState state;
  SequenceTimerSchema::Timer timer;
};

#endif //SG_MCU_SEQUENCE_TIMER_CORE_H
