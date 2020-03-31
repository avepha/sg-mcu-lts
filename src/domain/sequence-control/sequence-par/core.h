#include "TaskScheduler.h"
#include "domain/sensor/sensorPool.h"
#include "domain/sequence-control/sequenceState.h"
#include "domain/sequence-control/sequenceControl.h"
#include "domain/sequence/util/sequenceGpioChain.h"
#include "./model.h"
#include "logger/log.h"

#ifndef SG_MCU_SEQUENCE_PAR_CORE_H
#define SG_MCU_SEQUENCE_PAR_CORE_H

class SequenceParState : public SequenceState {
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

class SequenceParCore : public SequenceControl {
public:
  explicit SequenceParCore(SequenceGpioChain *gpioChain) : SequenceControl(SEQ_CTRL_PAR, gpioChain) {
    SequenceParModel model;
    SequenceParSchema parSchema = model.get();
    par = parSchema.par;
    sensorPool = SensorPool::instance();

    state.parSumInKJ = par.parSumInKJ;
  }

  ~SequenceParCore() override = default;

  SequenceParState getControlState() {
    return state;
  }

  bool controlTask() override {
    if (sensorPool->getAvailableStationBySensorId(Sensor::SENSORMAP["gs_par"]) <= 0) {
      Log::warn("seq-par", "No station available.");
      state.sensorValue = -1;
      return true;
    }

    state.sensorValue = sensorPool->getAverageStationBySensorId(Sensor::SENSORMAP["gs_par"]);

    switch (state.parState) {
      case SequenceParState::PAR_STATE_ACCUMULATING: {
        state.currentParSumInKJ += (state.sensorValue / 1000);
        state.isReachThreshold = state.currentParSumInKJ >= par.parSumInKJ;
        if (state.isReachThreshold) {
          if (!gpioChain->isEnabled()) gpioChain->enable();

          state.currentParSumInKJ = 0;
          state.parState = SequenceParState::PAR_STATE_WORKING;
          timestamp = millis();
        }
      }
      case SequenceParState::PAR_STATE_WORKING: {
        state.currentWorkingTimeInSecond = (millis() - timestamp) / 1000;
        if (state.currentWorkingTimeInSecond >= par.timing.workingTimeInSecond) {
          state.parState = SequenceParState::PAR_STATE_ACCUMULATING;
        }
      }
    }

    return true;
  }
private:
  uint32_t timestamp;
  SequenceParState state;
  SensorPool *sensorPool;
  SequenceParSchema::Par par;
};

#endif //SG_MCU_SEQUENCE_PAR_CORE_H
