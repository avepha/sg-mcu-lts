#include "../precondition.h"

#ifndef SG_MCU_PRECONDITION_TIMER_CORE_H
#define SG_MCU_PRECONDITION_TIMER_CORE_H

class PrecTimerCore : public Precondition {
public:
  bool resolve() {
    return true;
  }
};

#endif //SG_MCU_CORE_H
