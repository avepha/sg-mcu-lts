#include "TaskScheduler.h"
#include "domain/sensor/sensorPool.h"
#include "domain/channel-control/state.h"
#include "domain/channel-control/control.h"
#include "./model.h"
#include "logger/log.h"

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

  DynamicJsonDocument report() override {
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

class RangeCore : public Control {
public:
  explicit RangeCore(int channel) : Control(channel, CH_CTRL_RANGE) {
    RangeModel model;
    RangeSchema rangeSchema = model.get();
    range = rangeSchema.ranges[channel];

    sensorPool = SensorPool::instance();
    timeStamp = millis();
  }

  ~RangeCore() = default;

  RangeState getControlState() {
    return state;
  }

  bool controlTask() override {
    if (sensorPool->getAvailableStationBySensorId(range.sensor) <= 0) {
      Log::warn("ch-range", "No station available." + String(channel));
      state.sensorValue = -1;
      return true;
    }

    state.sensorValue = sensorPool->getAverageStationBySensorId(range.sensor);

    state.isReachThreshold = (range.greater) ? state.sensorValue >= range.lower && state.sensorValue >= range.upper
                                             : state.sensorValue <= range.lower && state.sensorValue <= range.upper;
    state.isTimingEnable = range.timing.enable;
    if (state.isTimingEnable) {
      switch (state.rangeState) {
        case RangeState::RANGE_STATE_WAITING : {
          state.currentWaitingTimeInSecond = (millis() - timeStamp) / 1000;
          if (state.currentWaitingTimeInSecond < range.timing.waitingTimeInSecond) {
            return true;
          }

          if (state.isReachThreshold) {
            // go to working state
            gpioCore->createGpioTaskTimeout(taskName, channel, range.timing.workingTimeInSecond * 1000);
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

          gpioCore->removeGpioTaskByChannel(channel);
          timeStamp = millis();
          state.rangeState = RangeState::RANGE_STATE_WAITING;
        }
      }
    }
    else {
      if (state.isReachThreshold) {
        gpioCore->createGpioTaskForever(taskName, channel);
      }
      else {
        gpioCore->removeGpioTaskByChannel(channel);
      }
    }

    return true;
  }

private:
  RangeState state;
  unsigned long timeStamp = 0;
  SensorPool *sensorPool;
  RangeSchema::Range range;
};

#endif //SG_MCU_CORE_H
