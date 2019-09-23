#include "validationError.h"
#include "combineContext.h"
#include "domain/resolvers.h"
#include "util/util.h"
#include "./permission.h"

#ifndef SG_MCU_CHANNEL_RESOLVERS_H
#define SG_MCU_CHANNEL_RESOLVERS_H

// @query
class query_channel : public Resolvers {
public:
  explicit query_channel(CombineContext *context) :
      Resolvers("channel", context, new permission_channel(context)) {};

  JsonDocument resolve(JsonObject reqData) override {
    int index = reqData["index"];
    ChannelSchema channelSchema = context->channelContext->channelModel->get();
    ChannelSchema::Channel channel = channelSchema.channels[index];

    DynamicJsonDocument data(1024);
    data["index"] = index;
    data["isActive"] = channel.isActive;
    JsonObject control = data.createNestedObject("control");
    control["type"] = channel.control.type;
    control["value"] = channel.control.value;

    JsonArray jarrPreconditions = data.createNestedArray("precondition");
    for (int i = 0; i < sizeof(channel.preconditions) / sizeof(channel.preconditions[0]); i++) {
      ChannelSchema::Channel::Precondition precondition = channel.preconditions[i];
      if (strcmp(precondition.type, "none") != 0) {
        JsonObject joPrecondition = jarrPreconditions.createNestedObject();
        joPrecondition["type"] = precondition.type;
        joPrecondition["value"] = precondition.value;
      }
    }

    return data;
  }
};

class mutation_channel_save : public Resolvers {
public:
  explicit mutation_channel_save(CombineContext *context) :
      Resolvers("channel_save", context, new permission_channel_save(context)) {};

  JsonDocument resolve(JsonObject reqData) override {
    ChannelSchema channelSchema = context->channelContext->channelModel->get();
    int index = reqData["index"];

    strcpy(channelSchema.channels[index].control.type, reqData["control"]["type"]);
    channelSchema.channels[index].control.value = reqData["control"]["value"];

    for (int i = 0 ; i < reqData["preconditions"].size(); i++) {
      strcpy(channelSchema.channels[index].preconditions[i].type, reqData["preconditions"][i]["type"]);
      channelSchema.channels[index].preconditions[i].value = reqData["preconditions"][i]["value"];
    }

    for (int i = reqData["preconditions"].size() ; i < 3; i++) {
      strcpy(channelSchema.channels[index].preconditions[i].type, "none");
      channelSchema.channels[index].preconditions[i].value = 0;
    }
    context->channelContext->channelModel->save(channelSchema);
    delay(10);

    ChannelSchema newChannelSchema = context->channelContext->channelModel->get();
    ChannelSchema::Channel channel = newChannelSchema.channels[index];

    DynamicJsonDocument data(1024);
    data["index"] = index;
    data["isActive"] = channel.isActive;
    JsonObject control = data.createNestedObject("control");
    control["type"] = channel.control.type;
    control["value"] = channel.control.value;

    JsonArray jarrPreconditions = data.createNestedArray("precondition");
    for (int i = 0; i < sizeof(channel.preconditions) / sizeof(channel.preconditions[0]); i++) {
      ChannelSchema::Channel::Precondition precondition = channel.preconditions[i];
      if (strcmp(precondition.type, "none") != 0) {
        JsonObject joPrecondition = jarrPreconditions.createNestedObject();
        joPrecondition["type"] = precondition.type;
        joPrecondition["value"] = precondition.value;
      }
    }
    return data;
  }
};


#endif //SG_MCU_RESOLVERS_H
