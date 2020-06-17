#include "./core.h"

#ifndef SG_MCU_GPIO_CONTEXT_H
#define SG_MCU_GPIO_CONTEXT_H

class NotificationContext {
public:
  GpioCore *core{};
  NotificationContext();
};

NotificationContext::NotificationContext() {
  core = GpioCore::instance();
}

#endif //SG_MCU_GPIO_CONTEXT_H
