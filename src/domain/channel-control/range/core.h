#include "TaskScheduler.h"
#include "domain/nsensor/core.h"
#include "domain/channel-control/state.h"
#include "domain/channel-control/control.h"
#include "./model.h"

#ifndef SG_MCU_RANGE_CORE_H
#define SG_MCU_RANGE_CORE_H

class RangeState : public State {
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

  JsonDocument report() {
    DynamicJsonDocument data(64);
    data["type"] = "range";
    data["sensorValue"] = sensorValue;
    data["isReachThreshold"] = isReachThreshold;
    if (isTimingEnable) {
      data["rangeState"] = (rangeState == RANGE_STATE_WAITING) ? "waiting" : "working";
      data["currentWaitingTimeInSecond"] = currentWaitingTimeInSecond;
      data["currentWorkingTimeInSecond"] = currentWorkingTimeInSecond;
    }
    return data;
  }
};

class RangeCore : public Control {
public:
  explicit RangeCore(int channel, void (*dWrite)(int channel, int value)) : Control(channel, CH_CTRL_RANGE, dWrite) {
    RangeModel model;
    RangeSchema rangeSchema = model.get();
    range = rangeSchema.ranges[channel];

    nSensorCore = NSensorCore::instance();
    timeStamp = millis();
  }

  ~RangeCore() override = default;

  RangeState getControlState() {
    return state;
  }

  bool controlTask() override {
    NSensor sensor = nSensorCore->getAverageSensor();
    state.sensorValue = sensor.sensors[range.sensor];

    state.isReachThreshold = (range.greater) ? state.sensorValue >= range.lower && state.sensorValue >= range.upper
                                             : state.sensorValue <= range.lower && state.sensorValue <= range.upper;

    if (state.isTimingEnable) {
      switch (state.rangeState) {
        case RangeState::RANGE_STATE_WAITING : {
          state.currentWaitingTimeInSecond = (millis() - timeStamp) / 1000;
          if (state.currentWaitingTimeInSecond < range.timing.waitingTimeInSecond) {
            return true;
          }

          if (state.isReachThreshold) {
            // go to working state
            dWrite(channel, HIGH);
            state.rangeState = RangeState::RANGE_STATE_WORKING;
          }

          timeStamp = millis();
        }
        case RangeState::RANGE_STATE_WORKING : {
          state.currentWorkingTimeInSecond = (millis() - timeStamp) / 1000;
          if (state.currentWorkingTimeInSecond < range.timing.workingTimeInSecond) {
            return true;
          }

          bool shouldStopWorking = (range.greater) ? state.sensorValue >= range.lower
                                                   : state.sensorValue <= range.upper;
          if (!shouldStopWorking) {
            return true;
          }

          dWrite(channel, LOW);
          timeStamp = millis();
          state.rangeState = RangeState::RANGE_STATE_WAITING;
        }
      }
    }
    else {
      state.isReachThreshold ? dWrite(channel, HIGH) : dWrite(channel, LOW);
    }

    return true;
  }

private:
  RangeState state;
  unsigned long timeStamp = 0;
  NSensorCore *nSensorCore;
  RangeSchema::Range range;
};

#endif //SG_MCU_CORE_H
