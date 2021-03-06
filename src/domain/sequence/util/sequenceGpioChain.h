#include <stack>
#include <TaskScheduler.h>
#include "domain/gpio/util/gpioTask.h"

#ifndef SG_MCU_SEQUENCE_GPIO_CHAIN_H
#define SG_MCU_SEQUENCE_GPIO_CHAIN_H

class SequenceGpioChain : public Task {
public:
  typedef struct {
    uint8_t channel = 0;
    uint32_t timeoutInSecond = 5;
  } ChannelAndTimeoutStruct;

  explicit SequenceGpioChain(std::string  name) : Task(500, TASK_FOREVER, &controlScheduler, false), taskName(std::move(name)) {
    gpioCore = GpioCore::instance();
  }

  void add(uint8_t channel, uint32_t timeoutInSec) {
    ChannelAndTimeoutStruct channelAndTimeoutStruct;
    channelAndTimeoutStruct.channel = channel;
    channelAndTimeoutStruct.timeoutInSecond = timeoutInSec;
    channelAndTimeouts.push_back(channelAndTimeoutStruct);
    totalWorkingTimeInSecond += timeoutInSec;
  }

  bool OnEnable() override {
    currentChannelIndex = 0;
    timestamp = millis();
    runGpioTask(channelAndTimeouts[currentChannelIndex]);
    return true;
  }

  bool Callback() override {
    if (deactivationFlag) {
      delete this;
      return false;
    }

    currentTimeInSecond = (millis() - timestamp) / 1000;
    if (currentTimeInSecond < channelAndTimeouts[currentChannelIndex].timeoutInSecond) {
      return true;
    }

    if (currentChannelIndex + 1 >= channelAndTimeouts.size()) {
      disable();
      return true;
    }

    currentChannelIndex++;

    timestamp = millis();
    runGpioTask(channelAndTimeouts[currentChannelIndex]);
    return true;
  }

  uint32_t getTotalWorkingTimeInSecond() {
    return totalWorkingTimeInSecond;
  }

  // to avoid race condition, between 2 processors
  // so deactivation process must be execute by main core
  // this is why setDeactivation method is in action
  void deactivate() {
    deactivationFlag = true;
  }
private:
  bool deactivationFlag = false;
  std::vector<ChannelAndTimeoutStruct> channelAndTimeouts{};
  std::string taskName;
  GpioCore *gpioCore = nullptr;
  int currentChannelIndex = 0;
  uint32_t timestamp = 0, currentTimeInSecond = 0, totalWorkingTimeInSecond = 0;

  void runGpioTask(ChannelAndTimeoutStruct channelAndTimeout) {
    gpioCore->createGpioTaskTimeout(taskName + String(channelAndTimeout.channel + 1).c_str(), channelAndTimeout.channel, channelAndTimeout.timeoutInSecond * 1000);
  }
};

#endif //SG_MCU_SEQUENCEGPIOCHAIN_H
