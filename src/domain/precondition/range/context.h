#include "./model.h"

#ifndef SG_MCU_PRECONDITION_RANGE_CONTEXT_H
#define SG_MCU_PRECONDITION_RANGE_CONTEXT_H

class PrecRangeContext {
public:
  PrecRangeModel *model;
  PrecRangeContext();
};

PrecRangeContext::PrecRangeContext() {
  model = new PrecRangeModel;
}

#endif //SG_MCU_CONTEXT_H
