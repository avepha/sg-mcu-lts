//
// Created by Alfarie-MBP on 2019-08-13.
//
#include "timer/model/precondition_timer_model.h"

#ifndef SG_MCU_PRECONDITION_CONTEXT_H
#define SG_MCU_PRECONDITION_CONTEXT_H

class PreConditionContext {
public:
  PreConditionTimerModel *timerModel;
  PreConditionContext();
};

PreConditionContext::PreConditionContext() {
  timerModel = new PreConditionTimerModel;
}

#endif //SG_MCU_CONTEXT_H
