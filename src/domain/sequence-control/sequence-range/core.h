#include "TaskScheduler.h"
#include "domain/sensor/sensorPool.h"
#include "domain/sequence-control/sequenceState.h"
#include "domain/sequence-control/sequenceControl.h"
#include "domain/sequence/util/sequenceGpioChain.h"
#include "./model.h"
#include "logger/log.h"

#ifndef SG_MCU_SEQUENCE_RANGE_CORE_H
#define SG_MCU_SEQUENCE_RANGE_CORE_H

class SequenceRangeState : public SequenceState {
public:
  enum RANGE_STATE_ENUM {
    RANGE_STATE_WAITING = 0,
    RANGE_STATE_WORKING = 1
  };
  bool isTimingEnable = false;
  RANGE_STATE_ENUM rangeState = RANGE_STATE_WAITING;
  float sensorValue = 0.0F;
  bool isReachThreshold = false;
  unsigned long currentWaitingTimeInSecond = 0;
  unsigned long currentWorkingTimeInSecond = 0;

  JsonDocument report() override {
    DynamicJsonDocument data(128);
    data["type"] = "range";
    data["sensorValue"] = sensorValue;
    data["isReachThreshold"] = isReachThreshold;
    if (isTimingEnable) {
      JsonObject timing = data.createNestedObject("timing");
      timing["rangeState"] = (rangeState == RANGE_STATE_WAITING) ? "waiting" : "working";
      timing["currentWaitingTimeInSecond"] = currentWaitingTimeInSecond;
      timing["currentWorkingTimeInSecond"] = currentWorkingTimeInSecond;
    }
    return data;
  }
};

class SequenceRangeCore : public SequenceControl {
public:
  explicit SequenceRangeCore(SequenceGpioChain *gpioChain) : SequenceControl(SEQ_CTRL_RANGE, gpioChain) {
    SequenceRangeModel model;
    SequenceRangeSchema rangeSchema = model.get();
    range = rangeSchema.range;

    sensorPool = SensorPool::instance();
    timeStamp = millis();
  }

  ~SequenceRangeCore() override = default;

  SequenceRangeState getControlState() {
    return state;
  }

  bool controlTask() override {
    if (sensorPool->getAvailableStationBySensorId(range.sensor) <= 0) {
      Log::warn("seq-range", "No station available.");
      state.sensorValue = -1;
      return true;
    }

    state.sensorValue = sensorPool->getAverageStationBySensorId(range.sensor);
    state.isTimingEnable = range.timing.enable;
    state.isReachThreshold = (range.greater) ? state.sensorValue >= range.lower && state.sensorValue >= range.upper
                                             : state.sensorValue <= range.lower && state.sensorValue <= range.upper;

    if (state.isTimingEnable) {
      switch (state.rangeState) {
        case SequenceRangeState::RANGE_STATE_WAITING : {
          state.currentWaitingTimeInSecond = (millis() - timeStamp) / 1000;
          if (state.currentWaitingTimeInSecond < range.timing.waitingTimeInSecond) {
            return true;
          }

          if (state.isReachThreshold) {
            // go to working state
            if (!gpioChain->isEnabled()) gpioChain->enable();
            state.rangeState = SequenceRangeState::RANGE_STATE_WORKING;
          }

          timeStamp = millis();
        }
        case SequenceRangeState::RANGE_STATE_WORKING : {
          state.currentWorkingTimeInSecond = (millis() - timeStamp) / 1000;
          if (state.currentWorkingTimeInSecond < gpioChain->getTotalWorkingTimeInSecond()) {
            return true;
          }

          bool shouldStopWorking = (range.greater) ? state.sensorValue >= range.lower
                                                   : state.sensorValue <= range.upper;
          if (!shouldStopWorking) {
            return true;
          }

          timeStamp = millis();
          state.rangeState = SequenceRangeState::RANGE_STATE_WAITING;
        }
      }
    }
    else { // if timing is disable
      if (state.isReachThreshold && !gpioChain->isEnabled()) {
        gpioChain->enable();
        return true;
      }
      else if (!state.isReachThreshold && gpioChain->isEnabled()) {
        gpioChain->disable();
        return true;
      }
    }

    return true;
  }

private:
  SequenceRangeState state;
  unsigned long timeStamp = 0;
  SensorPool *sensorPool;
  SequenceRangeSchema::Range range;
};

#endif //SG_MCU_SEQUENCE_RANGE_CORE_H
