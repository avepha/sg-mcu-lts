#include "./model.h"
#include "./core.h"

#ifndef SG_MCU_CONTINUOUS_CONTEXT_H
#define SG_MCU_CONTINUOUS_CONTEXT_H

class ContinuousContext {
public:
  ContinuousModel *model;
  ContinuousCore *core;
  ContinuousContext();
};

ContinuousContext::ContinuousContext() {
  model = new ContinuousModel;
  core = ContinuousCore::instance();
}

#endif //SG_MCU_CONTINUOUS_CONTEXT_H
