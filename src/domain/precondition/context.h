#include "timer/model/precondition_timer_model.h"
#include "domain/precondition/criteria/model.h"

#ifndef SG_MCU_PRECONDITION_CONTEXT_H
#define SG_MCU_PRECONDITION_CONTEXT_H

class PrecCriteriaContext {
public:
  PreConditionTimerModel *timerModel;
  PrecCriteriaModel *criteriaModel;
  PrecCriteriaContext();
};

PrecCriteriaContext::PrecCriteriaContext() {
  timerModel = new PreConditionTimerModel;
  criteriaModel = new PrecCriteriaModel;
}

#endif //SG_MCU_CONTEXT_H
