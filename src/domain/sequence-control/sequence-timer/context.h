#include "./model.h"
#ifndef SG_MCU_SEQUENCE_TIMER_CONTEXT_H
#define SG_MCU_SEQUENCE_TIMER_CONTEXT_H

class SequenceTimerContext {
public:
  SequenceTimerModel *model;
  SequenceTimerContext();
};

SequenceTimerContext::SequenceTimerContext() {
  model = new SequenceTimerModel;
}

#endif //SG_MCU_CONTEXT_H
