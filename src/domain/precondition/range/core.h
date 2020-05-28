#include "domain/channel-control/state.h"
#include "domain/sensor/sensorPool.h"
#include "../precondition.h"
#include "./model.h"

#ifndef SG_MCU_PRECONDITION_RANGE_CORE_H
#define SG_MCU_PRECONDITION_RANGE_CORE_H

class PrecRangeState : public State {
public:
  bool isReachThreshold = false;
  float sensorValue = 0;

  DynamicJsonDocument report() override {
    DynamicJsonDocument data(128);
    data["type"] = "precRange";
    data["isReachThreshold"] = isReachThreshold;
    data["sensorValue"] = sensorValue;
    return data;
  }
};

class PrecRangeCore : public Precondition {
public:
  explicit PrecRangeCore(int channel) : Precondition(PREC_RANGE) {
    sensorPool = SensorPool::instance();
    PrecRangeModel model;
    PrecRangeSchema precCriteriaSchema = model.get();
    range = precCriteriaSchema.ranges[channel];
  }

  bool resolve() override {
    if (sensorPool->getAvailableStationBySensorId(range.sensor) <= 0) {
      Log::warn("prec-range", "No station available.");
      state.sensorValue = -1;
      return false;
    }

    state.sensorValue = sensorPool->getAverageStationBySensorId(range.sensor);

    state.isReachThreshold = (state.sensorValue >= range.lower && state.sensorValue <= range.upper);

    return state.isReachThreshold;
  }

  PrecRangeState getPreconditionState() {
    return state;
  }

private:
  PrecRangeState state;
  SensorPool *sensorPool;
  PrecRangeSchema::Range range;
};

#endif //SG_MCU_CORE_H
