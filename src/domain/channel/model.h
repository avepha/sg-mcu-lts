#include "domain/control/channel-control/control.h"
#include "domain/precondition/precondition.h"
#include "domain/model.h"

#ifndef SG_MCU_CHANNEL_MODEL_H
#define SG_MCU_CHANNEL_MODEL_H

const int CHANNEL_ROM_ADDR = 1400;

struct ChannelSchema {
  struct Channel {
    bool isActive = false;

    struct Control {
      CONTROL_TYPE_ENUM type = CTRL_NONE;
      uint8_t value = 0; // manual
    } control;

    struct Precondition {
      PRECONDITION_TYPE_ENUM type = PREC_NONE;
      uint8_t value = 0;
    } preconditions[3];
  } channels[8];
}; // size 288

class ChannelModel : public RomModel<ChannelSchema> {
public:
  ChannelModel() : RomModel(CHANNEL_ROM_ADDR, "ChannelModel") {};
};


#endif //SG_MCU_MODEL_H
