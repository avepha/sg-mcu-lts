#include "timer/model/precondition_timer_model.h"
#include "criteria/model/precondition_criteria_model.h"

#ifndef SG_MCU_PRECONDITION_CONTEXT_H
#define SG_MCU_PRECONDITION_CONTEXT_H

class PreConditionContext {
public:
  PreConditionTimerModel *timerModel;
  PreConditionCriteriaModel *criteriaModel;
  PreConditionContext();
};

PreConditionContext::PreConditionContext() {
  timerModel = new PreConditionTimerModel;
  criteriaModel = new PreConditionCriteriaModel;
}

#endif //SG_MCU_CONTEXT_H
