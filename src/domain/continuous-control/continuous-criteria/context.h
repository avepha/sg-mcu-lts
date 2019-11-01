#include "./model.h"

#ifndef SG_MCU_CONTINUOUS_CRITERIA_CONTEXT_H
#define SG_MCU_CONTINUOUS_CRITERIA_CONTEXT_H

class ContinuousCriteriaContext {
public:
  ContinuousCriteriaModel *model;
  ContinuousCriteriaContext();
};

ContinuousCriteriaContext::ContinuousCriteriaContext() {
  model = new ContinuousCriteriaModel;
}

#endif //SG_MCU_CONTEXT_H
