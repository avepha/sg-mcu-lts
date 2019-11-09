#include "./model.h"

#ifndef SG_MCU_SEQUENCE_CRITERIA_CONTEXT_H
#define SG_MCU_SEQUENCE_CRITERIA_CONTEXT_H

class SequenceCriteriaContext {
public:
  SequenceCriteriaModel *model;
  SequenceCriteriaContext();
};

SequenceCriteriaContext::SequenceCriteriaContext() {
  model = new SequenceCriteriaModel;
}

#endif //SG_MCU_CONTEXT_H
