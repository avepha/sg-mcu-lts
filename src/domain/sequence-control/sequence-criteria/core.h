#include "TaskScheduler.h"
#include "domain/sensor/sensorPool.h"
#include "domain/sequence-control/sequenceState.h"
#include "domain/sequence-control/sequenceControl.h"
#include "domain/sequence/util/sequenceGpioChain.h"
#include "./model.h"

#include "util/debug.h"

#ifndef SG_MCU_SEQUENCE_CRITERIA_CORE_H
#define SG_MCU_SEQUENCE_CRITERIA_CORE_H

class SequenceCriteriaState : public SequenceState {
public:
  enum CRITERIA_STATE_ENUM {
    CRITERIA_STATE_WAITING = 0,
    CRITERIA_STATE_WORKING = 1
  };

  bool isTimingEnable = false;
  CRITERIA_STATE_ENUM criteriaState = CRITERIA_STATE_WAITING;
  float sensorValue = 0;
  bool isReachThreshold = false;
  unsigned long currentWaitingTimeInSecond = 0;
  unsigned long currentWorkingTimeInSecond = 0;

  JsonDocument report() override {
    DynamicJsonDocument data(256);
    data["type"] = "criteria";
    data["sensorValue"] = sensorValue;
    data["isReachThreshold"] = isReachThreshold;
    if (isTimingEnable) {
      JsonObject timing = data.createNestedObject("timing");
      timing["criteriaState"] = (criteriaState == CRITERIA_STATE_WAITING) ? "waiting" : "working";
      timing["currentWorkingTimeInSecond"] = currentWorkingTimeInSecond;
      timing["currentWaitingTimeInSecond"] = currentWaitingTimeInSecond;
    }
    return data;
  }
};

class SequenceCriteriaCore : public SequenceControl {
public:
  explicit SequenceCriteriaCore(SequenceGpioChain *gpioChain) : SequenceControl(SEQ_CTRL_CRITERIA, gpioChain) {
    SequenceCriteriaModel model;
    SequenceCriteriaSchema criteriaSchema = model.get();
    criteria = criteriaSchema.criteria;

    sensorPool = SensorPool::instance();
    timeStamp = millis();
  }

  ~SequenceCriteriaCore() override = default;

  SequenceCriteriaState getControlState() {
    return state;
  }

  bool controlTask() override {
    if (sensorPool->getAvailableStationBySensorId(criteria.sensor) <= 0) {
      state.sensorValue = -1;
      return true;
    }

    state.sensorValue = sensorPool->getAverageStationBySensorId(criteria.sensor);
    state.isTimingEnable = criteria.timing.enable;

    // check direction
    state.isReachThreshold = (criteria.greater) ? (state.sensorValue >= criteria.criteria)
                                                : (state.sensorValue <= criteria.criteria);

    //if timing is enables
    if (state.isTimingEnable) {
      switch (state.criteriaState) {
        case SequenceCriteriaState::CRITERIA_STATE_WAITING: {
          state.currentWaitingTimeInSecond = (millis() - timeStamp) / 1000;
          if (state.currentWaitingTimeInSecond < criteria.timing.waitingTimeInSecond) {
            return true;
          }

          if (state.isReachThreshold) {
            // go to working state
            if (!gpioChain->isEnabled()) gpioChain->enable();
            state.criteriaState = SequenceCriteriaState::CRITERIA_STATE_WORKING;
          }
          else {
            // stay at waiting state
            state.criteriaState = SequenceCriteriaState::CRITERIA_STATE_WAITING;
          }
          timeStamp = millis();
        }
        case SequenceCriteriaState::CRITERIA_STATE_WORKING: {
          state.currentWorkingTimeInSecond = (millis() - timeStamp) / 1000;
          if (state.currentWorkingTimeInSecond < gpioChain->getTotalWorkingTimeInSecond()) {
            return true;
          }

          // go to waiting state
          timeStamp = millis();
          state.criteriaState = SequenceCriteriaState::CRITERIA_STATE_WAITING;
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
  SequenceCriteriaState state;
  unsigned long timeStamp = 0;
  SensorPool *sensorPool;
  SequenceCriteriaSchema::Criteria criteria;
};

#endif //SG_MCU_SEQUENCE_CRITERIA_CORE_H
