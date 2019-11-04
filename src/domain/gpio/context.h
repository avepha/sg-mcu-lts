#include "./core.h"

#ifndef SG_MCU_GPIO_CONTEXT_H
#define SG_MCU_GPIO_CONTEXT_H

class GpioContext {
public:
  GpioCore *core{};
  GpioContext();
};

GpioContext::GpioContext() {
  core = GpioCore::instance();
}

#endif //SG_MCU_GPIO_CONTEXT_H
