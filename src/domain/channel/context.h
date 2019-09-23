#include "model/model.h"

#ifndef SG_MCU_CHANNEL_CONTEXT_H
#define SG_MCU_CHANNEL_CONTEXT_H


class ChannelContext {
public:
  ChannelModel *channelModel;
  ChannelContext();
};

ChannelContext::ChannelContext() {
  channelModel = new ChannelModel();
}

#endif //SG_MCU_CONTEXT_H
