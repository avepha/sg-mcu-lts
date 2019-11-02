#include <array>

#include "./model.h"

#include "domain/precondition/preconditionFactory.h"
#include "domain/channel-control/controlFactory.h"

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

  static std::array<int, 8> getChannelGpioState() {
    return channelGpioState;
  }

  Control* getChannelControlAt(int channel) {
    return channelControl[channel];
  }

  void checkAndActivateControlType(ChannelSchema::Channel channelData, int channel) {
    // check and remove control
    if (channelControl[channel] != nullptr) {
      channelControl[channel]->disable();
      delete channelControl[channel];
      channelControl[channel] = nullptr;
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

      return;
    }

    Control *control = ControlFactory::getControl(channelData.control.type, channel, &dWrite);
    if (control != nullptr) {
      channelControl[channel] = control;

      for (int i = 0; i < sizeof(channelData.preconditions) / sizeof(channelData.preconditions[0]); i++) {
        Precondition *precondition = PreconditionFactory::getPrecondition(channelData.preconditions[i].type, channel);
        if (precondition != nullptr) {
          channelControl[channel]->addPrecondition(precondition);
        }
      }

      channelControl[channel]->enable();
    }
    else {
      dWrite(channel, LOW);
    }
  }

private:
  static std::array<int, 8> channelGpioState;

  static void dWrite(int channel = 0, int value = LOW) {
    if (channelGpioState[channel] == value)
      return;

    channelGpioState[channel] = value;
    digitalWrite(CHANNEL_GPIO_MAP[channel], value);
  }

  std::array<Control*, 8> channelControl{};

};

std::array<int, 8> ChannelCore::channelGpioState = {{LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW}};

#endif //SG_MCU_CHANNEL_CORE_H
