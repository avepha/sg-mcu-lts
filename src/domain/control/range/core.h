#include "TaskScheduler.h"
#include "domain/nsensor/core.h"
#include "domain/control/state.h"
#include "../control.h"
#include "./model.h"

#ifndef SG_MCU_RANGE_CORE_H
#define SG_MCU_RANGE_CORE_H

class RangeState : public State {
public:
  JsonDocument report() {
    DynamicJsonDocument data(64);
    return data;
  }
};

class RangeCore : public Control {
public:
  explicit RangeCore(int channel, void (*dWrite)(int channel, int value)) : Control(channel, CTRL_RANGE, dWrite) {
    RangeModel model;
    RangeSchema rangeSchema = model.get();
    range = rangeSchema.ranges[channel];

    nSensorCore = NSensorCore::instance();
    timeStamp = millis();
  }

  ~RangeCore() override = default;

  RangeState getControlState() {
    return state;
  }

  bool controlTask() override {
    return true;
  }

private:
  RangeState state;
  unsigned long timeStamp = 0;
  NSensorCore *nSensorCore;
  RangeSchema::Range range;
};

#endif //SG_MCU_CORE_H
