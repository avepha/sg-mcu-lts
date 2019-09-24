#include "model/model.h"
#include "core/channel_core.h"

#ifndef SG_MCU_CHANNEL_CONTEXT_H
#define SG_MCU_CHANNEL_CONTEXT_H

class ChannelContext {
public:
  ChannelModel *channelModel;
  ChannelCore *channelCore;
  ChannelContext();
};

ChannelContext::ChannelContext() {
  channelModel = new ChannelModel();
  channelCore = new ChannelCore();
}

#endif //SG_MCU_CONTEXT_H
