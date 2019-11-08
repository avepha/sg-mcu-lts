#include "./model.h"

#ifndef SG_MCU_CONTINUOUS_RANGE_CONTEXT_H
#define SG_MCU_CONTINUOUS_RANGE_CONTEXT_H

class ContinuousRangeContext {
public:
  ContinuousRangeModel *model;
  ContinuousRangeContext();
};

ContinuousRangeContext::ContinuousRangeContext() {
  model = new ContinuousRangeModel;
}

#endif //SG_MCU_CONTINUOUS_RANGE_CONTEXT_H
