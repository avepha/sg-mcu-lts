#include "model/criteria_model.h"

#ifndef SG_MCU_CRITERIA_CONTEXT_H
#define SG_MCU_CRITERIA_CONTEXT_H

class CriteriaContext {
public:
  CriteriaModel *model;
  CriteriaContext();
};

CriteriaContext::CriteriaContext() {
  model = new CriteriaModel;
}


#endif //SG_MCU_CONTEXT_H
