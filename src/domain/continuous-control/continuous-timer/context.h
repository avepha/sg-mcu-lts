#include "./model.h"

#ifndef SG_MCU_CONTINUOUS_TIMER_CONTEXT_H
#define SG_MCU_CONTINUOUS_TIMER_CONTEXT_H

class ContinuousTimerContext {
public:
  ContinuousTimerModel *model;
  ContinuousTimerContext();
};

ContinuousTimerContext::ContinuousTimerContext() {
  model = new ContinuousTimerModel;
}

#endif //SG_MCU_CONTEXT_H
