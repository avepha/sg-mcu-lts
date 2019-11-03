#include "domain/gpio/util/gpio-task.h"

#ifndef SG_MCU_GPIO_CORE_H
#define SG_MCU_GPIO_CORE_H

class GpioCore {
public:
  static GpioCore* instance()
  {
    if (!s_instance)
      s_instance = new GpioCore;
    return s_instance;
  }

private:
  GpioCore() {
    for (int gpio : CHANNEL_GPIO_MAP) {
      pinMode(gpio, OUTPUT);
    }
  };
  static GpioCore *s_instance;
};

GpioCore *GpioCore::s_instance = nullptr;

#endif //SG_MCU_GPIO_CORE_H
