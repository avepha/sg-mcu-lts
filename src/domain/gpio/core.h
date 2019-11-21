#include "domain/gpio/util/gpioTask.h"

#ifndef SG_MCU_GPIO_CORE_H
#define SG_MCU_GPIO_CORE_H

class GpioCore {
public:
  static GpioCore *instance();

  GpioTask* createGpioTaskForever(const std::string& name, uint8_t channel);

  GpioTask* createGpioTaskTimeout(const std::string& name, uint8_t channel, uint16_t timeoutInMs);

  bool removeGpioTaskByName(const std::string& name);
  bool removeGpioTaskByChannel(uint8_t channel);
  bool removeGpioTaskAll();

  std::map<std::string, GpioTask*> getGpioTaskMap() {
    return gpioTaskMap;
  }

  std::array<uint8_t , 8> getGpioState() {
    return channelGpioState;
  }

private:
  GpioCore();

  static GpioCore *s_instance;

  static std::array<uint8_t , 8> channelGpioState;

  static void dWrite(int channel = 0, int value = LOW);

  static void selfDestructGpioTask(GpioTask *gpioTask);

  static std::map<std::string, GpioTask*> gpioTaskMap;
};

// Implementation
GpioCore *GpioCore::s_instance = nullptr;

std::array<uint8_t , 8> GpioCore::channelGpioState = {{LOW}};

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

  if (gpioTask) {
    delete gpioTask; // [-Wdelete-non-virtual-dtor] occur
  }
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

GpioTask* GpioCore::createGpioTaskTimeout(const std::string& name, uint8_t channel, uint16_t timeoutInMs) {
  if (gpioTaskMap.find(name) != gpioTaskMap.end()) {
    return nullptr;
  }

  auto *gpioTask = new GpioTask(name, channel, timeoutInMs, &dWrite, &selfDestructGpioTask);
  gpioTaskMap[name] = gpioTask;
  gpioTask->enableDelayed();
  return gpioTask;
}

GpioTask* GpioCore::createGpioTaskForever(const std::string &name, uint8_t channel) {
  if (gpioTaskMap.find(name) != gpioTaskMap.end()) {
    return nullptr;
  }

  auto *gpioTask = new GpioTask(name, channel, &dWrite, &selfDestructGpioTask);
  gpioTaskMap[name] = gpioTask;
  gpioTask->enable();
  return gpioTask;
}

bool GpioCore::removeGpioTaskByName(const std::string& name) {
  if (gpioTaskMap.find(name) == gpioTaskMap.end()) {
    return false;
  }

  selfDestructGpioTask(gpioTaskMap[name]);
  return true;
}

bool GpioCore::removeGpioTaskByChannel(uint8_t channel) {
  if (gpioTaskMap.size() <= 0) {
    return false;
  }

  for (std::pair<std::string, GpioTask*> task: gpioTaskMap) {
    if (channel == task.second->getChannel()) {
      selfDestructGpioTask(gpioTaskMap[task.first]);
      return true;
    }
  }

  return false;
}

bool GpioCore::removeGpioTaskAll() {
  for (int i = 0; i < sizeof(CHANNEL_GPIO_MAP) / sizeof(CHANNEL_GPIO_MAP[0]); i++) {
    removeGpioTaskByChannel(i);
  }
  return true;
}

#endif //SG_MCU_GPIO_CORE_H
