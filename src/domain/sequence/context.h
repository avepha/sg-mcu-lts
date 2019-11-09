#include "./model.h"
#include "./core.h"

#ifndef SG_MCU_SEQUENCE_CONTEXT_H
#define SG_MCU_SEQUENCE_CONTEXT_H

class SequenceContext {
public:
  SequenceModel *model;
  SequenceCore *core;
  SequenceContext();
};

SequenceContext::SequenceContext() {
  model = new SequenceModel;
  core = SequenceCore::instance();
}

#endif //SG_MCU_SEQUENCE_CONTEXT_H
