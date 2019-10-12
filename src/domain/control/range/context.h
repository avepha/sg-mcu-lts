#include "./model.h"

#ifndef SG_MCU_RANGE_CONTEXT_H
#define SG_MCU_RANGE_CONTEXT_H

class RangeContext {
public:
  RangeModel *model;
  RangeContext();
};

RangeContext::RangeContext() {
  model = new RangeModel;
}


#endif //SG_MCU_CONTEXT_H
