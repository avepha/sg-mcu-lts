#include "TaskScheduler.h"
#include "domain/nsensor/core.h"
#include "domain/control/state.h"
#include "domain/control/channel-control/control.h"
#include "./model.h"

#ifndef SG_MCU_CRITERIA_CORE_H
#define SG_MCU_CRITERIA_CORE_H

class CriteriaState : public State {
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

  JsonDocument report() {
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

class CriteriaCore : public Control {
public:
  explicit CriteriaCore(int channel, void (*dWrite)(int channel, int value)) : Control(channel, CTRL_CRITERIA, dWrite) {
    CriteriaModel model;
    CriteriaSchema criteriaSchema = model.get();
    criteria = criteriaSchema.criterias[channel];

    nSensorCore = NSensorCore::instance();
    timeStamp = millis();
  }

  ~CriteriaCore() override = default;

  CriteriaState getControlState() {
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
    // if timing is enables
    if (state.isTimingEnable) {
      switch (state.criteriaState) {
        case CriteriaState::CRITERIA_STATE_WAITING: {
          state.currentWaitingTimeInSecond = (millis() - timeStamp) / 1000;
          if (state.currentWaitingTimeInSecond < criteria.timing.waitingTimeInSecond) {
            return true;
          }

          if (state.isReachThreshold) {
            // go to working state
            dWrite(channel, HIGH);
            state.criteriaState = CriteriaState::CRITERIA_STATE_WORKING;
          }
          else {
            // stay at waiting state
            state.criteriaState = CriteriaState::CRITERIA_STATE_WAITING;
          }
          timeStamp = millis();
        }
        case CriteriaState::CRITERIA_STATE_WORKING: {
          state.currentWorkingTimeInSecond = (millis() - timeStamp) / 1000;
          if (state.currentWorkingTimeInSecond < criteria.timing.workingTimeInSecond) {
            return true;
          }

          // go to waiting state
          dWrite(channel, LOW);
          timeStamp = millis();
          state.criteriaState = CriteriaState::CRITERIA_STATE_WAITING;
        }
      }
    }
    else { // if timing is disable
      state.isReachThreshold ? dWrite(channel, HIGH) : dWrite(channel, LOW);
    }

    return true;
  }

private:
  CriteriaState state;
  unsigned long timeStamp = 0;
  NSensorCore *nSensorCore;
  CriteriaSchema::Criteria criteria;
};

#endif //SG_MCU_CORE_H
