#include <stack>
#include <TaskScheduler.h>
#include "domain/gpio/util/gpioTask.h"

#ifndef SG_MCU_CONTINUOUS_GPIO_CHAIN_H
#define SG_MCU_CONTINUOUS_GPIO_CHAIN_H

class ContinuousGpioChain : public Task {
public:
  typedef struct {
    uint8_t channel = 0;
    uint32_t timeoutInSecond = 5;
  } ChannelAndTimeoutStruct;

  ContinuousGpioChain() : Task(500, TASK_FOREVER, &controlScheduler, false) {
    gpioCore = GpioCore::instance();
  }

  void add(uint8_t channel, uint32_t timeoutInSec) {
    ChannelAndTimeoutStruct channelAndTimeoutStruct;
    channelAndTimeoutStruct.channel = channel;
    channelAndTimeoutStruct.timeoutInSecond = timeoutInSec;
    channelAndTimeoutStack.push(channelAndTimeoutStruct);
    totalWorkingTimeInSecond += timeoutInSec;
  }

  bool OnEnable() override {
    timestamp = millis();
    runGpioTask();
    return true;
  }

  bool Callback() override {
    if (channelAndTimeoutStack.empty()) {
      disable();
    }

    currentTimeInSecond = (millis() - timestamp) / 1000;
    if (currentTimeInSecond >= currentChannelAndTimeout.timeoutInSecond) {
      runGpioTask();
    }
    return true;
  }

  uint32_t getTotalWorkingTimeInSecond() {
    return totalWorkingTimeInSecond;
  }

private:
  std::stack<ChannelAndTimeoutStruct> channelAndTimeoutStack{};
  ChannelAndTimeoutStruct currentChannelAndTimeout;
  GpioCore *gpioCore = nullptr;
  uint32_t timestamp = 0, currentTimeInSecond = 0, totalWorkingTimeInSecond = 0;

  void runGpioTask() {
    currentChannelAndTimeout = channelAndTimeoutStack.top();
    channelAndTimeoutStack.pop();
    std::string taskName = ("continuous-channel-" + String(currentChannelAndTimeout.channel)).c_str();
    gpioCore->createGpioTaskTimeout(taskName, currentChannelAndTimeout.channel, currentChannelAndTimeout.timeoutInSecond * 1000);
  }
};

#endif //SG_MCU_CONTINUOUSGPIOCHAIN_H
