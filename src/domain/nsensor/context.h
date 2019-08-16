#include "core/nsensor_core.h"

#ifndef SG_MCU_NSENSOR_CONTEXT_H
#define SG_MCU_NSENSOR_CONTEXT_H


class NSensorContext {
public:
  NSensorCore *core;
  NSensorContext();
};

NSensorContext::NSensorContext() {
  core = new NSensorCore;
}


#endif //SG_MCU_CONTEXT_H
