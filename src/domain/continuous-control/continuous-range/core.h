#include "TaskScheduler.h"
#include "domain/nsensor/core.h"
#include "domain/continuous-control/continuousState.h"
#include "domain/continuous-control/continuousControl.h"
#include "domain/continuous/util/continuousGpioChain.h"
#include "./model.h"

#ifndef SG_MCU_CONTINUOUS_RANGE_CORE_H
#define SG_MCU_CONTINUOUS_RANGE_CORE_H

class ContinuousRangeState : public ContinuousState {
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
      data["rangeState"] = (rangeState == RANGE_STATE_WAITING) ? "waiting" : "working";
      data["currentWaitingTimeInSecond"] = currentWaitingTimeInSecond;
      data["currentWorkingTimeInSecond"] = currentWorkingTimeInSecond;
    }
    return data;
  }
};

class ContinuousRangeCore : public ContinuousControl {
public:
  explicit ContinuousRangeCore(ContinuousGpioChain *gpioChain) : ContinuousControl(CON_CTRL_RANGE, gpioChain) {
    ContinuousRangeModel model;
    ContinuousRangeSchema rangeSchema = model.get();
    range = rangeSchema.range;

    nSensorCore = NSensorCore::instance();
    timeStamp = millis();
  }

  ~ContinuousRangeCore() override = default;

  ContinuousRangeState getControlState() {
    return state;
  }

  bool controlTask() override {
    NSensor sensor = nSensorCore->getAverageSensor();
    state.sensorValue = sensor.sensors[range.sensor];
    state.isTimingEnable = range.timing.enable;
    state.isReachThreshold = (range.greater) ? state.sensorValue >= range.lower && state.sensorValue >= range.upper
                                             : state.sensorValue <= range.lower && state.sensorValue <= range.upper;

    if (state.isTimingEnable) {
      switch (state.rangeState) {
        case ContinuousRangeState::RANGE_STATE_WAITING : {
          state.currentWaitingTimeInSecond = (millis() - timeStamp) / 1000;
          if (state.currentWaitingTimeInSecond < range.timing.waitingTimeInSecond) {
            return true;
          }

          if (state.isReachThreshold) {
            // go to working state
            if (!gpioChain->isEnabled()) gpioChain->enable();
            state.rangeState = ContinuousRangeState::RANGE_STATE_WORKING;
          }

          timeStamp = millis();
        }
        case ContinuousRangeState::RANGE_STATE_WORKING : {
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
          state.rangeState = ContinuousRangeState::RANGE_STATE_WAITING;
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
  ContinuousRangeState state;
  unsigned long timeStamp = 0;
  NSensorCore *nSensorCore;
  ContinuousRangeSchema::Range range;
};

#endif //SG_MCU_CONTINUOUS_RANGE_CORE_H
