#include "TaskScheduler.h"
#include "domain/nsensor/core.h"
#include "domain/channel-control/state.h"
#include "domain/channel-control/control.h"
#include "./model.h"

#ifndef SG_MCU_PAR_CORE_H
#define SG_MCU_PAR_CORE_H

class ParState : public State {
public:
  enum PAR_STATE_ENUM {
    PAR_STATE_ACCUMULATING = 0,
    PAR_STATE_WORKING = 1
  };

  float sensorValue = 0;
  float currentParSumInKJ = 0;
  float parSumInKJ = 0;
  bool isReachThreshold = false;
  PAR_STATE_ENUM parState = PAR_STATE_ACCUMULATING;
  uint32_t currentWorkingTimeInSecond = 0;

  JsonDocument report() override {
    DynamicJsonDocument data(128);
    data["type"] = "par";
    data["isReachThreshold"] = isReachThreshold;
    data["sensorValue"] = sensorValue;
    data["currentParSumInKJ"] = currentParSumInKJ;
    data["parSumInKJ"] = parSumInKJ;
    data["parState"] = parState == PAR_STATE_ACCUMULATING ? "accumulation" : "working";
    if (parState == PAR_STATE_WORKING) {
      data["currentWorkingTimeInSecond"] = currentWorkingTimeInSecond;
    }
    return data;
  }
};

class ParCore : public Control {
public:
  explicit ParCore(int channel) : Control(channel, CH_CTRL_PAR) {
    ParModel model;
    ParSchema parSchema = model.get();
    par = parSchema.pars[channel];
    parSensorIndex = parSchema.parSensorIndex;
    nSensorCore = NSensorCore::instance();

    state.parSumInKJ = par.parSumInKJ;
  }

  ~ParCore() override = default;

  ParState getControlState() {
    return state;
  }

  bool controlTask() override {
    NSensor averageSensor = nSensorCore->getAverageSensor();
    state.sensorValue = averageSensor.sensors[parSensorIndex];

    switch (state.parState) {
      case ParState::PAR_STATE_ACCUMULATING: {
        state.currentParSumInKJ += (state.sensorValue / 1000);
        state.isReachThreshold = state.currentParSumInKJ >= par.parSumInKJ;
        if (state.isReachThreshold) {
          gpioTask = gpioCore->createGpioTaskTimeout(taskName, channel, par.timing.workingTimeInSecond * 1000);

          state.currentParSumInKJ = 0;
          state.parState = ParState::PAR_STATE_WORKING;
          timestamp = millis();
        }
      }
      case ParState::PAR_STATE_WORKING: {
        state.currentWorkingTimeInSecond = (millis() - timestamp) / 1000;
        if (state.currentWorkingTimeInSecond >= par.timing.workingTimeInSecond) {
          state.parState = ParState::PAR_STATE_ACCUMULATING;
        }
      }
    }

    return true;
  }

private:
  uint8_t parSensorIndex;
  uint32_t timestamp;
  ParState state;
  ParSchema::Par par;
  NSensorCore *nSensorCore;
  GpioTask *gpioTask = nullptr;
};

#endif //SG_MCU_CORE_H
