#include "./core.h"
#include "./model.h"

#ifndef SG_MCU_STATION_CONTEXT_H
#define SG_MCU_STATION_CONTEXT_H

class StationContext {
public:
  StationCore *core;
  StationModel *model;
  StationContext();
};

StationContext::StationContext() {
  core = new StationCore();
  model = new StationModel();
}

#endif //SG_MCU_CONTEXT_H
