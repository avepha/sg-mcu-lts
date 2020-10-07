#include "./model.h"
#include "./core.h"

#ifndef SG_MCU_PSTATION_CONTEXT_H
#define SG_MCU_PSTATION_CONTEXT_H

class PStationContext {
public:
  PStationModel *model;
  PStationCore *core;
  PStationContext();
};

PStationContext::PStationContext() {
  core = PStationCore::instance();
  model = new PStationModel();
}

#endif //SG_MCU_CONTEXT_H
