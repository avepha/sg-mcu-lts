#include "./model.h"

#ifndef SG_MCU_PRECONDITION_CONTEXT_H
#define SG_MCU_PRECONDITION_CONTEXT_H

class PrecCriteriaContext {
public:
  PrecCriteriaModel *model;
  PrecCriteriaContext();
};

PrecCriteriaContext::PrecCriteriaContext() {
  model = new PrecCriteriaModel;
}

#endif //SG_MCU_CONTEXT_H
