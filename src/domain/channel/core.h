#include "domain/control/control.h"
#include "domain/precondition/precondition.h"
#include "./model.h"

#ifndef SG_MCU_CHANNEL_CORE_H
#define SG_MCU_CHANNEL_CORE_H

class ChannelCore {
public:
  ChannelCore() {
    // initialize pin
    for (int i = 0 ; i < sizeof(CHANNEL_GPIO_MAP) / sizeof(CHANNEL_GPIO_MAP[0]); i++) {
      pinMode(CHANNEL_GPIO_MAP[i], OUTPUT);
    }

    ChannelModel channelModel;
    ChannelSchema channelSchema = channelModel.get();

    for (int i = 0 ; i < sizeof(channelSchema.channels) / sizeof(channelSchema.channels[0]); i++) {
      if (channelSchema.channels[i].control.type == CTRL_MANUAL) {
        switch (channelSchema.channels[i].control.value) {
          case 1:
            digitalWrite(CHANNEL_GPIO_MAP[i], HIGH);
            break;
          default:
            digitalWrite(CHANNEL_GPIO_MAP[i], LOW);
            break;
        }
      }
      if (channelSchema.channels[i].control.type == CTRL_TIMER) {

      }

      else {
        digitalWrite(CHANNEL_GPIO_MAP[i], LOW);
      }
    }
  }

private:
  struct ChannelControl {
    Control *control = nullptr;
    Precondition preconditions[3];
  } channelControl[8];

};

#endif //SG_MCU_CHANNEL_CORE_H
