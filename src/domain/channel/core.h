#include <array>

#include "./model.h"

#include "domain/control/channel-control/control.h"
#include "domain/control/channel-control/timer/core.h"
#include "domain/control/channel-control/criteria/core.h"
#include "domain/control/channel-control/range/core.h"

#include "domain/precondition/precondition.h"
#include "domain/precondition/criteria/core.h"
#include "domain/precondition/timer/core.h"
#include "domain/precondition/range/core.h"

#ifndef SG_MCU_CHANNEL_CORE_H
#define SG_MCU_CHANNEL_CORE_H

class ChannelCore {
public:
  struct ChannelControl {
    Control *control = nullptr;
  };

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

  ChannelControl getChannelControlAt(int channel) {
    return channelControl[channel];
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
      channelControl[channel].control->enable();
    }
    else if (channelData.control.type == CTRL_CRITERIA) {
      channelControl[channel].control = new CriteriaCore(channel, &dWrite);

      for (int i = 0; i < sizeof(channelData.preconditions) / sizeof(channelData.preconditions[0]); i++) {
        if (channelData.preconditions[i].type == PREC_CRITERIA) {
          channelControl[channel].control->addPrecondition(new PrecCriteriaCore(channel));
        }
        else if (channelData.preconditions[i].type == PREC_TIMER) {
          channelControl[channel].control->addPrecondition(new PrecTimerCore(channel));
        }
        else if (channelData.preconditions[i].type == PREC_RANGE) {
          channelControl[channel].control->addPrecondition(new PrecRangeCore(channel));
        }
      }
      channelControl[channel].control->enable();
    }
    else if (channelData.control.type == CTRL_RANGE) {
      channelControl[channel].control = new RangeCore(channel, &dWrite);

      for (int i = 0; i < sizeof(channelData.preconditions) / sizeof(channelData.preconditions[0]); i++) {
        if (channelData.preconditions[i].type == PREC_CRITERIA) {
          channelControl[channel].control->addPrecondition(new PrecCriteriaCore(channel));
        }
        else if (channelData.preconditions[i].type == PREC_TIMER) {
          channelControl[channel].control->addPrecondition(new PrecTimerCore(channel));
        }
        else if (channelData.preconditions[i].type == PREC_RANGE) {
          channelControl[channel].control->addPrecondition(new PrecRangeCore(channel));
        }
      }
      channelControl[channel].control->enable();
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

  std::array<ChannelControl, 8> channelControl;

};

std::array<int, 8> ChannelCore::channelGpioState = {{LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW}};

#endif //SG_MCU_CHANNEL_CORE_H
