#include "./gpioState.h"

#ifndef SG_MCU_PGPIO_CONTEXT_H
#define SG_MCU_PGPIO_CONTEXT_H

class PGpioContext {
public:
  GpioState *gpioState;
  PGpioContext();
};

PGpioContext::PGpioContext() {
  gpioState = GpioState::instance();
}

#endif //SG_MCU_GPIO_CONTEXT_H
