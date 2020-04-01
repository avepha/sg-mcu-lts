#include "domain/channel-control/state.h"
#include "domain/sensor/sensorPool.h"
#include "../precondition.h"
#include "./model.h"

#ifndef SG_MCU_PRECONDITION_CRITERIA_CORE_H
#define SG_MCU_PRECONDITION_CRITERIA_CORE_H

class PrecCriteriaState : public State {
public:
  bool isReachThreshold = false;
  float sensorValue = 0;

  DynamicJsonDocument report() override {
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
    sensorPool = SensorPool::instance();
    PrecCriteriaModel model;
    PrecCriteriaSchema precCriteriaSchema = model.get();
    criteria = precCriteriaSchema.criterias[channel];
  }

  bool resolve() override {
    if (sensorPool->getAvailableStationBySensorId(criteria.sensor)) {
      state.sensorValue = -1;
      return false;
    }

    state.sensorValue = sensorPool->getAverageStationBySensorId(criteria.sensor);
    state.isReachThreshold = (criteria.greater) ? (state.sensorValue >= criteria.criteria)
                                                : (state.sensorValue <= criteria.criteria);

    return state.isReachThreshold;
  }

  PrecCriteriaState getPreconditionState() {
    return state;
  }

private:
  PrecCriteriaState state;
  SensorPool *sensorPool;
  PrecCriteriaSchema::Criteria criteria;
};

#endif //SG_MCU_CORE_H
