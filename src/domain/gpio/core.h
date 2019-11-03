#include "domain/gpio/util/gpio-task.h"

#ifndef SG_MCU_GPIO_CORE_H
#define SG_MCU_GPIO_CORE_H

class GpioCore {
public:
  static GpioCore *instance();

  bool createGpioTaskForever(const std::string& name, uint8_t channel);

  bool createGpioTaskTimeout(const std::string& name, uint8_t channel, uint16_t timeout);

  bool removeGpioTask(const std::string& name);

private:
  GpioCore();

  static GpioCore *s_instance;

  static std::array<int, 8> channelGpioState;

  static void dWrite(int channel = 0, int value = LOW);

  static void selfDestructGpioTask(GpioTask *gpioTask);

  static std::map<std::string, GpioTask*> gpioTaskMap;
};

// Implementation
GpioCore *GpioCore::s_instance = nullptr;

std::array<int, 8> GpioCore::channelGpioState = {{LOW}};

std::map<std::string, GpioTask*> GpioCore::gpioTaskMap{};

void GpioCore::dWrite(int channel, int value) {
  if (channelGpioState[channel] == value)
    return;

  channelGpioState[channel] = value;
  digitalWrite(CHANNEL_GPIO_MAP[channel], value);
}

void GpioCore::selfDestructGpioTask(GpioTask *gpioTask) {
  std::string taskName = gpioTask->getTaskName();
  if (gpioTaskMap.find(taskName) != gpioTaskMap.end()) {
    gpioTaskMap.erase(taskName);
  }

  delete &gpioTask;
}

GpioCore *GpioCore::instance() {
  if (!s_instance)
    s_instance = new GpioCore;
  return s_instance;
}

GpioCore::GpioCore() {
  for (int gpio : CHANNEL_GPIO_MAP) {
    pinMode(gpio, OUTPUT);
  }
}

bool GpioCore::createGpioTaskTimeout(const std::string& name, uint8_t channel, uint16_t timeout) {
  if (gpioTaskMap.find(name) != gpioTaskMap.end()) {
    return false;
  }

  auto *gpioTask = new GpioTask(name, channel, timeout, &dWrite, &selfDestructGpioTask);
  gpioTaskMap[name] = gpioTask;
  gpioTask->enable();
  return true;
}

bool GpioCore::createGpioTaskForever(const std::string &name, uint8_t channel) {
  if (gpioTaskMap.find(name) != gpioTaskMap.end()) {
    return false;
  }

  auto *gpioTask = new GpioTask(name, channel, &dWrite, &selfDestructGpioTask);
  gpioTaskMap[name] = gpioTask;
  gpioTask->enable();
  return true;
}

bool GpioCore::removeGpioTask(const std::string& name) {
  if (gpioTaskMap.find(name) != gpioTaskMap.end()) {
    return false;
  }
  selfDestructGpioTask(gpioTaskMap[name]);
  return true;
}

#endif //SG_MCU_GPIO_CORE_H
