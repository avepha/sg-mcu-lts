#include "./model.h"

#ifndef SG_MCU_SEQUENCE_PAR_CONTEXT_H
#define SG_MCU_SEQUENCE_PAR_CONTEXT_H

class SequenceParContext {
public:
  SequenceParModel *model;
  SequenceParContext();
};

SequenceParContext::SequenceParContext() {
  model = new SequenceParModel;
}

#endif //SG_MCU_CONTEXT_H
