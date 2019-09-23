#include "domain/model.h"

#ifndef SG_MCU_CHANNEL_MODEL_H
#define SG_MCU_CHANNEL_MODEL_H

const int CHANNEL_ROM_ADDR = 2000;

struct ChannelSchema {
  struct channel {
    bool isActive = false;
    struct control {
      char type[10] = "manual";
      uint8_t value = 0;
    } control;
    struct precondition {
      char type[10] = "none";
      uint8_t value = 0;
    } precondition;
  } channels[8];
};

class ChannelModel : public RomModel<ChannelSchema> {
public:
  ChannelModel() : RomModel(CHANNEL_ROM_ADDR, "ChannelModel") {};
};


#endif //SG_MCU_MODEL_H
