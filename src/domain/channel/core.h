#include <array>

#include "./model.h"

#include "domain/precondition/preconditionFactory.h"
#include "domain/channel-control/controlFactory.h"
#include "domain/p_gpio/gpioTask.h"

#ifndef SG_MCU_CHANNEL_CORE_H
#define SG_MCU_CHANNEL_CORE_H

class ChannelCore {
public:
  static ChannelCore *instance() {
    if (!s_instance)
      s_instance = new ChannelCore;
    return s_instance;
  }

  ChannelCore() {
    for (int i = 0; i < sizeof(CHANNEL_GPIO_MAP) / sizeof(CHANNEL_GPIO_MAP[0]); i++) {
      pinMode(CHANNEL_GPIO_MAP[i], OUTPUT);
    }
  }

  Control *getControlByChannel(int channel) {
    return channelControl[channel];
  }

  void checkAndActivateControl(ChannelSchema::Channel channelData, int channel) {
    // check and remove control
    if (channelControl[channel] != nullptr) {
      channelControl[channel]->deactivate();
      channelControl[channel] = nullptr;
    }

    if (!channelData.isActive) {
        PGpioTask::setState(channel, LOW);
      return;
    }

    if (channelData.control.type == CH_CTRL_MANUAL) {
      std::string manualTaskName = String("CHMNL" + String(channel)).c_str();
      switch (channelData.control.value) {
        case 1:
          PGpioTask::setState(channel, HIGH);
          break;
        default:
          PGpioTask::setState(channel, LOW);
          break;
      }

      return;
    }

    Control *control = ControlFactory::getControl(channelData.control.type, channel);
    if (control != nullptr) {
      channelControl[channel] = control;

      for (int i = 0; i < sizeof(channelData.preconditions) / sizeof(channelData.preconditions[0]); i++) {
        Precondition *precondition = PreconditionFactory::getPrecondition(channelData.preconditions[i].type, channel);
        if (precondition != nullptr) {
          channelControl[channel]->addPrecondition(precondition);
        }
      }

      channelControl[channel]->enableDelayed();
    }
    else {
      PGpioTask::setState(channel, LOW);
    }
  }

  void activateControls() {
    ChannelModel channelModel;
    ChannelSchema channelSchema = channelModel.get();

    for (int i = 0; i < sizeof(channelSchema.channels) / sizeof(channelSchema.channels[0]); i++) {
      checkAndActivateControl(channelSchema.channels[i], i);
    }
  }

  void deactivateControls() {
    for (int channel = 0; channel < channelControl.size(); channel++) {
      if (channelControl[channel] != nullptr) {
        channelControl[channel]->deactivate();
        channelControl[channel] = nullptr;
      }
    }
  }

private:
  static ChannelCore *s_instance;
  std::array<Control *, 8> channelControl{};
};

ChannelCore *ChannelCore::s_instance = nullptr;

#endif //SG_MCU_CHANNEL_CORE_H
