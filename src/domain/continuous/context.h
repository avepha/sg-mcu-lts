#include "./model.h"


#ifndef SG_MCU_CONTINUOUS_CONTEXT_H
#define SG_MCU_CONTINUOUS_CONTEXT_H

class ContinuousContext {
public:
  ContinuousModel *model;
  ContinuousContext();
};

ContinuousContext::ContinuousContext() {
  model = new ContinuousModel();
}

#endif //SG_MCU_CONTINUOUS_CONTEXT_H
