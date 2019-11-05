#include "TaskScheduler.h"
#include "domain/nsensor/core.h"
#include "domain/continuous-control/continuousState.h"
#include "domain/continuous-control/continuousControl.h"
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

class ContinuousCriteriaCore : public ContinuousControl {
public:
  explicit ContinuousCriteriaCore(int channel) : ContinuousControl(channel, CON_CTRL_CRITERIA) {
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
    Serial.println("running");
  }

private:
  ContinuousCriteriaState state;
  unsigned long timeStamp = 0;
  NSensorCore *nSensorCore;
  ContinuousCriteriaSchema::Criteria criteria;
};

#endif //SG_MCU_CONTINUOUS_CRITERIA_CORE_H
