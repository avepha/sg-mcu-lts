#include "core/sensor_core.h"
#include "model/sensor_model.h"

#ifndef SG_MCU_SENSOR_CONTEXT_H
#define SG_MCU_SENSOR_CONTEXT_H


class SensorContext {
public:
  SensorCore *core;
  SensorModel *model;
  SensorContext();
};

SensorContext::SensorContext() {
  core = new SensorCore;
  model = new SensorModel;
}


#endif //SG_MCU_CONTEXT_H
