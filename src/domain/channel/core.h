#include <array>

#include "./model.h"

#include "domain/precondition/preconditionFactory.h"
#include "domain/channel-control/controlFactory.h"

#ifndef SG_MCU_CHANNEL_CORE_H
#define SG_MCU_CHANNEL_CORE_H

class ChannelCore {
public:
  static ChannelCore* instance()
  {
    if (!s_instance)
      s_instance = new ChannelCore;
    return s_instance;
  }

  ChannelCore() {
    gpioCore = GpioCore::instance();
    for (int i = 0; i < sizeof(CHANNEL_GPIO_MAP) / sizeof(CHANNEL_GPIO_MAP[0]); i++) {
      pinMode(CHANNEL_GPIO_MAP[i], OUTPUT);
    }
  }

  Control* getControlByChannel(int channel) {
    return channelControl[channel];
  }

  void checkAndActivateControl(ChannelSchema::Channel channelData, int channel) {
    // check and remove control
    if (channelControl[channel] != nullptr) {
      channelControl[channel]->disable();
      delete channelControl[channel];
      channelControl[channel] = nullptr;
    }

    if (!channelData.isActive) {
      gpioCore->removeGpioTaskByChannel(channel);
      return;
    }

    if (channelData.control.type == CH_CTRL_MANUAL) {
      std::string manualTaskName = String("manual-" + String(channel)).c_str();
      switch (channelData.control.value) {
        case 1:
          gpioCore->createGpioTaskForever(manualTaskName, channel);
          break;
        default:
          gpioCore->removeGpioTaskByName(manualTaskName);
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

      channelControl[channel]->enable();
    }
    else {
      gpioCore->removeGpioTaskByChannel(channel);
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
    for (int channel = 0 ; channel < channelControl.size(); channel++) {
      if (channelControl[channel] != nullptr) {
        channelControl[channel]->disable();
        delete channelControl[channel];
        channelControl[channel] = nullptr;
      }
    }
  }

private:
  static ChannelCore *s_instance;
  std::array<Control*, 8> channelControl{};
  GpioCore *gpioCore;
};

ChannelCore *ChannelCore::s_instance = nullptr;

#endif //SG_MCU_CHANNEL_CORE_H
