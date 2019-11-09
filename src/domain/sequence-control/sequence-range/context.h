#include "./model.h"

#ifndef SG_MCU_SEQUENCE_RANGE_CONTEXT_H
#define SG_MCU_SEQUENCE_RANGE_CONTEXT_H

class SequenceRangeContext {
public:
  SequenceRangeModel *model;
  SequenceRangeContext();
};

SequenceRangeContext::SequenceRangeContext() {
  model = new SequenceRangeModel;
}

#endif //SG_MCU_SEQUENCE_RANGE_CONTEXT_H
