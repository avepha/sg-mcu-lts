#include "./model.h"

#ifndef SG_MCU_PAR_CONTEXT_H
#define SG_MCU_PAR_CONTEXT_H

class ParContext {
public:
  ParModel *model;
  ParContext();
};

ParContext::ParContext() {
  model = new ParModel;
}

#endif //SG_MCU_PAR_CONTEXT_H
