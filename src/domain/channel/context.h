#include "./model.h"
#include "./core.h"

#ifndef SG_MCU_CHANNEL_CONTEXT_H
#define SG_MCU_CHANNEL_CONTEXT_H

class ChannelContext {
public:
  ChannelModel *model;
  ChannelCore *core;
  ChannelContext();
};

ChannelContext::ChannelContext() {
  model = new ChannelModel();
  core = new ChannelCore();
}

#endif //SG_MCU_CONTEXT_H
