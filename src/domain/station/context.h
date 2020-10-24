#include "./model.h"
#include "./core.h"

#ifndef SG_MCU_STATION_CONTEXT_H
#define SG_MCU_STATION_CONTEXT_H

class StationContext {
public:
  StationModel *model;
  StationCore *core;
  StationContext();
};

StationContext::StationContext() {
  core = StationCore::instance();
  model = new StationModel();
}

#endif //SG_MCU_CONTEXT_H
