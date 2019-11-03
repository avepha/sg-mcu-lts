#include "./model.h"
#include "./core.h"

#ifndef SG_MCU_CONTROL_CONTEXT_H
#define SG_MCU_CONTROL_CONTEXT_H

class ControlContext {
public:
  ControlModel *model;
  ControlCore *core;
  ControlContext();
};

ControlContext::ControlContext() {
  model = new ControlModel;
  core = new ControlCore;
}

#endif //SG_MCU_CONTROL_CONTEXT_H
