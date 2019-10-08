#include <array>

#include "domain/control/control.h"
#include "domain/control/timer/core.h"

#include "domain/precondition/precondition.h"
#include "./model.h"

#ifndef SG_MCU_CHANNEL_CORE_H
#define SG_MCU_CHANNEL_CORE_H

class ChannelCore {
public:
  ChannelCore() {
    // initialize pin
    for (int i = 0; i < sizeof(CHANNEL_GPIO_MAP) / sizeof(CHANNEL_GPIO_MAP[0]); i++) {
      pinMode(CHANNEL_GPIO_MAP[i], OUTPUT);
    }

    ChannelModel channelModel;
    ChannelSchema channelSchema = channelModel.get();

    for (int i = 0; i < sizeof(channelSchema.channels) / sizeof(channelSchema.channels[0]); i++) {
      checkAndActivateControlType(channelSchema.channels[i], i);
    }
  }

  std::array<int, 8> getChannelState() {
    return channelState;
  }

  void checkAndActivateControlType(ChannelSchema::Channel channelData, int channel) {
    // check and remove control
    if (channelControl[channel].control != nullptr) {
      channelControl[channel].control->disable();
      delete channelControl[channel].control;
      channelControl[channel].control = nullptr;
    }

    if (!channelData.isActive) {
      dWrite(channel, LOW);
      return;
    }

    if (channelData.control.type == CTRL_MANUAL) {
      switch (channelData.control.value) {
        case 1:
          dWrite(channel, HIGH);
          break;
        default:
          dWrite(channel, LOW);
          break;
      }
    }
    else if (channelData.control.type == CTRL_TIMER) {
      channelControl[channel].control = new TimerCore(channel, &dWrite);
    }
    else {
      dWrite(channel, LOW);
    }
  }

  static std::array<int, 8> channelState;

  static void dWrite(int channel = 0, int value = LOW) {
    channelState[channel] = value;
    digitalWrite(CHANNEL_GPIO_MAP[channel], value);
  }

private:
  struct ChannelControl {
    Control *control = nullptr;
    Precondition preconditions[3];
  } channelControl[8];
};

std::array<int, 8> ChannelCore::channelState = {{LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW}};

#endif //SG_MCU_CHANNEL_CORE_H
