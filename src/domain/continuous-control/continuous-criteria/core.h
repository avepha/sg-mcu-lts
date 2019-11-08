#include "TaskScheduler.h"
#include "domain/nsensor/core.h"
#include "domain/continuous-control/continuousState.h"
#include "domain/continuous-control/continuousControl.h"
#include "domain/continuous/util/continuousGpioChain.h"
#include "./model.h"

#ifndef SG_MCU_CONTINUOUS_CRITERIA_CORE_H
#define SG_MCU_CONTINUOUS_CRITERIA_CORE_H

class ContinuousCriteriaState : public ContinuousState {
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
      data["criteriaState"] = (criteriaState == CRITERIA_STATE_WAITING) ? "waiting" : "working";
      data["currentWorkingTimeInSecond"] = currentWorkingTimeInSecond;
      data["currentWaitingTimeInSecond"] = currentWaitingTimeInSecond;
    }
    return data;
  }
};

class ContinuousCriteriaCore : public ContinuousControl {
public:
  explicit ContinuousCriteriaCore(ContinuousGpioChain *gpioChain) : ContinuousControl(CON_CTRL_CRITERIA, gpioChain) {
    ContinuousCriteriaModel model;
    ContinuousCriteriaSchema criteriaSchema = model.get();
    criteria = criteriaSchema.criteria;

    nSensorCore = NSensorCore::instance();
    timeStamp = millis();
  }

  ~ContinuousCriteriaCore() override = default;

  ContinuousCriteriaState getControlState() {
    return state;
  }

  bool controlTask() override {
//  CriteriaModel::ShowModel(criteria, channel);
    NSensor averageSensor = nSensorCore->getAverageSensor();
    state.sensorValue = averageSensor.sensors[criteria.sensor];
    state.isTimingEnable = criteria.timing.enable;

    // check direction
    state.isReachThreshold = (criteria.greater) ? (state.sensorValue >= criteria.criteria)
                                                : (state.sensorValue <= criteria.criteria);

//     if timing is enables
    if (state.isTimingEnable) {
      switch (state.criteriaState) {
        case ContinuousCriteriaState::CRITERIA_STATE_WAITING: {
          state.currentWaitingTimeInSecond = (millis() - timeStamp) / 1000;
          if (state.currentWaitingTimeInSecond < criteria.timing.waitingTimeInSecond) {
            return true;
          }

          if (state.isReachThreshold) {
            // go to working state
            if (!gpioChain->isEnabled()) gpioChain->enable();
            state.criteriaState = ContinuousCriteriaState::CRITERIA_STATE_WORKING;
          }
          else {
            // stay at waiting state
            state.criteriaState = ContinuousCriteriaState::CRITERIA_STATE_WAITING;
          }
          timeStamp = millis();
        }
        case ContinuousCriteriaState::CRITERIA_STATE_WORKING: {
          state.currentWorkingTimeInSecond = (millis() - timeStamp) / 1000;
          if (state.currentWorkingTimeInSecond < gpioChain->getTotalWorkingTimeInSecond()) {
            return true;
          }

          // go to waiting state
          timeStamp = millis();
          state.criteriaState = ContinuousCriteriaState::CRITERIA_STATE_WAITING;
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
  ContinuousCriteriaState state;
  unsigned long timeStamp = 0;
  NSensorCore *nSensorCore;
  ContinuousCriteriaSchema::Criteria criteria;
};

#endif //SG_MCU_CONTINUOUS_CRITERIA_CORE_H
