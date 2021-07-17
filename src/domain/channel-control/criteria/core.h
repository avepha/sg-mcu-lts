#include "TaskScheduler.h"
#include "domain/sensor/sensorPool.h"
#include "domain/channel-control/state.h"
#include "domain/channel-control/control.h"
#include "./model.h"
#include "logger/log.h"

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

  DynamicJsonDocument report() override {
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

class CriteriaCore : public Control {
public:
  explicit CriteriaCore(int channel) : Control(channel, CH_CTRL_CRITERIA) {
    CriteriaModel model;
    CriteriaSchema criteriaSchema = model.get();
    criteria = criteriaSchema.criterias[channel];

    sensorPool = SensorPool::instance();
    timeStamp = millis();
  }

  ~CriteriaCore() = default;

  CriteriaState getControlState() {
    return state;
  }

  bool controlTask(bool isPrecPass) override {
    if (!isPrecPass) {
      // set state to zero
      Log::debug("ch-criteria", "not-pass-prec " + String(channel));
      state.isReachThreshold = false;
      gpioTask->low();

      return true; // stop if precondition is not pass
    }

    if (sensorPool->getAvailableStationBySensorId(criteria.sensor) <= 0) {
      Log::warn("ch-criteria", "No station available." + String(channel));
      state.sensorValue = -1;
      gpioTask->low();
      return true;
    }

    state.sensorValue = sensorPool->getAverageStationBySensorId(criteria.sensor);

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
            gpioTask->low();
            return true;
          }

          if (state.isReachThreshold) {
            // go to working state
            gpioTask->high();
            state.criteriaState = CriteriaState::CRITERIA_STATE_WORKING;
          }
          else {
            // stay at waiting state
            state.criteriaState = CriteriaState::CRITERIA_STATE_WAITING;
          }
          timeStamp = millis();
          return true;
        }
        case CriteriaState::CRITERIA_STATE_WORKING: {
          state.currentWorkingTimeInSecond = (millis() - timeStamp) / 1000;
          if (state.currentWorkingTimeInSecond < criteria.timing.workingTimeInSecond) {
            gpioTask->high();
            return true;
          }

          // go to waiting state
          gpioTask->low();
          timeStamp = millis();
          state.criteriaState = CriteriaState::CRITERIA_STATE_WAITING;

          return true;
        }
      }
    }
    else { // if timing is disable
      if (state.isReachThreshold) {
        gpioTask->high();
      }
      else {
        gpioTask->low();
      }
    }

    return true;
  }

private:
  CriteriaState state;
  SensorPool *sensorPool;
  unsigned long timeStamp = 0;
  CriteriaSchema::Criteria criteria;
};

#endif //SG_MCU_CORE_H
