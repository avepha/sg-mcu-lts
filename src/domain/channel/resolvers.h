#include "validationError.h"
#include "combineContext.h"
#include "domain/resolvers.h"
#include "util/util.h"
#include "./permission.h"
#include "../control/util/resolveControlEnum.h"
#include "../precondition/util/resolvePreconditionEnum.h"


#ifndef SG_MCU_CHANNEL_RESOLVERS_H
#define SG_MCU_CHANNEL_RESOLVERS_H

// @query
class query_channel : public Resolvers {
public:
  explicit query_channel(CombineContext *context) :
      Resolvers(
          "channel", context,
          new permission_channel(context)) {};

  JsonDocument resolve(JsonObject reqData) override {
    int index = reqData["index"];
    ChannelSchema channelSchema = context->channelContext->channelModel->get();
    ChannelSchema::Channel channel = channelSchema.channels[index];

    DynamicJsonDocument data(1024);
    data["index"] = index;
    data["isActive"] = channel.isActive;
    JsonObject control = data.createNestedObject("control");
    control["type"] = ControlEnumToString(channel.control.type);
    control["value"] = channel.control.value;

    JsonArray jarrPreconditions = data.createNestedArray("preconditions");
    for (int i = 0; i < sizeof(channel.preconditions) / sizeof(channel.preconditions[0]); i++) {
      ChannelSchema::Channel::Precondition precondition = channel.preconditions[i];
      if (precondition.type != PREC_NONE) {
        JsonObject joPrecondition = jarrPreconditions.createNestedObject();
        joPrecondition["type"] = PreconditionEnumToString(precondition.type);
        joPrecondition["value"] = precondition.value;
      }
    }

    return data;
  }
};

class query_gpio : public Resolvers {
public:
  explicit query_gpio(CombineContext *context) :
      Resolvers("gpio", context) {};

  JsonDocument resolve(JsonObject reqData) override {
    std::array<int, 8> channelState = context->channelContext->channelCore->getChannelState();

    DynamicJsonDocument data(1024);
    for (int i = 0; i < channelState.size(); i++) {
      JsonObject channel = data.createNestedObject();
      channel["channel"] = i;
      channel["state"] = channelState[i] == HIGH ? "HIGH" : "LOW";
    }

    return data;
  }
};

class mutation_channel_save : public Resolvers {
public:
  explicit mutation_channel_save(CombineContext *context) :
      Resolvers(
          "channel_save", context,
          new permission_channel_save(context)) {};

  JsonDocument resolve(JsonObject reqData) override {
    ChannelSchema channelSchema = context->channelContext->channelModel->get();
    int index = reqData["index"];

    channelSchema.channels[index].control.type = ControlStringToEnum(reqData["control"]["type"]);
    channelSchema.channels[index].control.value = reqData["control"]["value"];

    // set precondition to providing args
    for (int i = 0; i < reqData["preconditions"].size(); i++) {
      channelSchema.channels[index].preconditions[i].type = PreconditionStringToEnum(
          reqData["preconditions"][i]["type"]);
      channelSchema.channels[index].preconditions[i].value = reqData["preconditions"][i]["value"];
    }

    // set precondition to none if providing args size < 3
    for (int i = reqData["preconditions"].size(); i < 3; i++) {
      channelSchema.channels[index].preconditions[i].type = PREC_NONE;
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
    control["type"] = ControlEnumToString(channel.control.type);
    control["value"] = channel.control.value;

    JsonArray jarrPreconditions = data.createNestedArray("preconditions");
    for (int i = 0; i < sizeof(channel.preconditions) / sizeof(channel.preconditions[0]); i++) {
      ChannelSchema::Channel::Precondition precondition = channel.preconditions[i];
      if (precondition.type != PREC_NONE) {
        JsonObject joPrecondition = jarrPreconditions.createNestedObject();
        joPrecondition["type"] = PreconditionEnumToString(precondition.type);
        joPrecondition["value"] = precondition.value;
      }
    }
    return data;
  }
};

class channel_activate : public Resolvers {
public:
  explicit channel_activate(CombineContext *context) :
      Resolvers(
          "channel_activate",
          context,
          new permission_channel_activate(context)) {};

  JsonDocument resolve(JsonObject reqData) override {
    int index = reqData["index"];
    boolean isActive = reqData["isActive"];
    ChannelSchema channelSchema = context->channelContext->channelModel->get();
    channelSchema.channels[index].isActive = isActive;

    context->channelContext->channelModel->save(channelSchema);
    delay(10);

    ChannelSchema newChannelSchema = context->channelContext->channelModel->get();
    DynamicJsonDocument data(1024);
    data["index"] = index;
    data["isActive"] = newChannelSchema.channels[index].isActive;

    // index = channel 0 - 7
    context->channelContext->channelCore->checkAndActivateControlType(newChannelSchema.channels[index], index);

    return data;
  }
};

#endif //SG_MCU_RESOLVERS_H
