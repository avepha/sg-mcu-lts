#include "domain/channel-control/state.h"
#include "domain/nsensor/core.h"
#include "../precondition.h"
#include "./model.h"

#ifndef SG_MCU_PRECONDITION_RANGE_CORE_H
#define SG_MCU_PRECONDITION_RANGE_CORE_H

class PrecRangeState : public State {
public:
  bool isReachThreshold = false;
  float sensorValue = 0;

  JsonDocument report() override {
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
    nSensorCore = NSensorCore::instance();
    PrecRangeModel model;
    PrecRangeSchema precCriteriaSchema = model.get();
    range = precCriteriaSchema.ranges[channel];
  }

  bool resolve() override {
    NSensor averageSensor = nSensorCore->getAverageSensor();
    state.sensorValue = averageSensor.sensors[range.sensor];
    state.isReachThreshold = (state.sensorValue >= range.lower && state.sensorValue <= range.upper);

    return state.isReachThreshold;
  }

  PrecRangeState getPreconditionState() {
    return state;
  }

private:
  PrecRangeState state;
  NSensorCore *nSensorCore;
  PrecRangeSchema::Range range;
};

#endif //SG_MCU_CORE_H
