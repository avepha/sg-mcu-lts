#include "domain/model.h"

#ifndef SG_MCU_CHANNEL_MODEL_H
#define SG_MCU_CHANNEL_MODEL_H

const int CHANNEL_ROM_ADDR = 1900;

struct ChannelSchema {
  struct Channel {
    bool isActive = false;
    struct control {
      char type[10] = "manual";
      uint8_t value = 0;
    } control;
    struct Precondition {
      char type[10] = "none";
      uint8_t value = 0;
    } preconditions[3];
  } channels[8];
}; // size 184

class ChannelModel : public RomModel<ChannelSchema> {
public:
  ChannelModel() : RomModel(CHANNEL_ROM_ADDR, "ChannelModel") {};
};


#endif //SG_MCU_MODEL_H
