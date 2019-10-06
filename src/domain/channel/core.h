#include "domain/control/control.h"
#include "domain/precondition/precondition.h"

#ifndef SG_MCU_CHANNEL_CORE_H
#define SG_MCU_CHANNEL_CORE_H

class ChannelCore {
public:
  ChannelCore() {}

private:
  struct ChannelControl {
    Control *control = nullptr;
    Precondition preconditions[3];
  } channelControl[8];
};

#endif //SG_MCU_CHANNEL_CORE_H
