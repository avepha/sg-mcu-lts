#include "./model.h"

#ifndef SG_MCU_PRECONDITION_TIMER_CONTEXT_H
#define SG_MCU_PRECONDITION_TIMER_CONTEXT_H

class PrecTimerContext {
public:
  PrecTimerModel *model;
  PrecTimerContext();
};

PrecTimerContext::PrecTimerContext() {
  model = new PrecTimerModel;
}

#endif //SG_MCU_CONTEXT_H
