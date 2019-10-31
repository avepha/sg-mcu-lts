#include "domain/channel-control/state.h"
#include "domain/nsensor/core.h"
#include "../precondition.h"
#include "./model.h"

#ifndef SG_MCU_PRECONDITION_CRITERIA_CORE_H
#define SG_MCU_PRECONDITION_CRITERIA_CORE_H

class PrecCriteriaState : public State {
public:
  bool isReachThreshold = false;
  float sensorValue = 0;

  JsonDocument report() override {
    DynamicJsonDocument data(128);
    data["type"] = "precCriteria";
    data["isReachThreshold"] = isReachThreshold;
    data["sensorValue"] = sensorValue;
    return data;
  }
};

class PrecCriteriaCore : public Precondition {
public:
  explicit PrecCriteriaCore(int channel) : Precondition(PREC_CRITERIA) {
    nSensorCore = NSensorCore::instance();
    PrecCriteriaModel model;
    PrecCriteriaSchema precCriteriaSchema = model.get();
    criteria = precCriteriaSchema.criterias[channel];
  }

  bool resolve() override {
    NSensor averageSensor = nSensorCore->getAverageSensor();
    state.sensorValue = averageSensor.sensors[criteria.sensor];
    state.isReachThreshold = (criteria.greater) ? (state.sensorValue >= criteria.criteria)
                                                : (state.sensorValue <= criteria.criteria);

    return state.isReachThreshold;
  }

  PrecCriteriaState getPreconditionState() {
    return state;
  }

private:
  PrecCriteriaState state;
  NSensorCore *nSensorCore;
  PrecCriteriaSchema::Criteria criteria;
};

#endif //SG_MCU_CORE_H
