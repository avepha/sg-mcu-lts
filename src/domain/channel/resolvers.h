#include "validationError.h"
#include "combineContext.h"
#include "domain/resolvers.h"
#include "util/util.h"

#ifndef SG_MCU_CHANNEL_RESOLVERS_H
#define SG_MCU_CHANNEL_RESOLVERS_H

// @query
class query_channel : public Resolvers {
public:
  explicit query_channel(CombineContext *context) : Resolvers("channel", context) {};

  JsonDocument resolve(JsonObject reqData) override {
    if (reqData["index"].isNull()) {
      InvalidInputError err("index field is not specified.");
      throw err;
    }

    if (reqData["index"] < 0 || reqData["index"] > 7) {
      InvalidInputError err("index out of range.");
      throw err;
    }

    int index = reqData["index"];
    ChannelSchema channelSchema = context->channelContext->channelModel->get();
    ChannelSchema::Channel channel = channelSchema.channels[index];

    DynamicJsonDocument data(1024);
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
  explicit mutation_channel_save(CombineContext *context) : Resolvers("channel_save", context) {};

  JsonDocument resolve(JsonObject reqData) override {
    if (reqData["index"].isNull()) {
      InvalidInputError err("index field is not specified.");
      throw err;
    }

    if (reqData["index"] < 0 || reqData["index"] > 7) {
      InvalidInputError err("index out of range.");
      throw err;
    }

    if (reqData["control"].isNull() || reqData["control"]["type"].isNull() || reqData["control"]["value"].isNull()) {
      InvalidInputError err("control field must have type and value.");
      throw err;
    }

    if (reqData["preconditions"].isNull() || !reqData["preconditions"].is<JsonArray>()) {
      InvalidInputError err("preconditions field must be an array.");
      throw err;
    }

    for (int i = 0 ; i < reqData["preconditions"].as<JsonArray>().size(); i++) {
      JsonObject jo = reqData["preconditions"][0];
      if (jo["type"].isNull() || jo["value"].isNull()) {
        InvalidInputError err("precondition must have field type and value");
        throw err; 
      }
    }

    DynamicJsonDocument data(1024);
    return data;
  }
};


#endif //SG_MCU_RESOLVERS_H
