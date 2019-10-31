#include "./model.h"

#ifndef SG_MCU_CONTROL_TIMER_CONTEXT_H
#define SG_MCU_CONTROL_TIMER_CONTEXT_H

class TimerContext {
public:
  TimerModel *model;
  TimerContext();
};

TimerContext::TimerContext() {
  model = new TimerModel;
}

#endif //SG_MCU_CONTEXT_H
