#include <stack>
#include <TaskScheduler.h>
#include "domain/gpio/util/gpioTask.h"
#include "util/debug.h"

#ifndef SG_MCU_SEQUENCE_GPIO_CHAIN_H
#define SG_MCU_SEQUENCE_GPIO_CHAIN_H

class SequenceGpioChain : public Task {
public:
  typedef struct {
    uint8_t channel = 0;
    uint32_t timeoutInSecond = 5;
  } ChannelAndTimeoutStruct;

  SequenceGpioChain() : Task(1000, TASK_FOREVER, &controlScheduler, false) {
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
    Debug::Print("gpio-chain" + String(currentChannelIndex));
    currentTimeInSecond = (millis() - timestamp) / 1000;
    if (currentTimeInSecond < channelAndTimeouts[currentChannelIndex].timeoutInSecond) {
      return true;
    }

    if (currentChannelIndex + 1 >= channelAndTimeouts.size()) {
      return false;
    }

    currentChannelIndex++;

    timestamp = millis();
    runGpioTask(channelAndTimeouts[currentChannelIndex]);
    return true;
  }

  uint32_t getTotalWorkingTimeInSecond() {
    return totalWorkingTimeInSecond;
  }

private:
  std::vector<ChannelAndTimeoutStruct> channelAndTimeouts{};
  GpioCore *gpioCore = nullptr;
  int currentChannelIndex = 0;
  uint32_t timestamp = 0, currentTimeInSecond = 0, totalWorkingTimeInSecond = 0;

  void runGpioTask(ChannelAndTimeoutStruct channelAndTimeout) {
    std::string taskName = ("sequence-channel-" + String(channelAndTimeout.channel)).c_str();
    gpioCore->createGpioTaskTimeout(taskName, channelAndTimeout.channel, channelAndTimeout.timeoutInSecond * 1000);
  }
};

#endif //SG_MCU_SEQUENCEGPIOCHAIN_H
