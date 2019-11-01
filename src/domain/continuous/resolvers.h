#include "validationError.h"
#include "combineContext.h"
#include "domain/resolvers.h"
#include "util/util.h"
#include "./permission.h"
#include "domain/continuous-control/util/resolveContinuousControlEnum.h"
#include "domain/precondition/util/resolvePreconditionEnum.h"

#ifndef SG_MCU_CONTINUOUS_RESOLVERS_H
#define SG_MCU_CONTINUOUS_RESOLVERS_H

// @query
class query_continuous : public Resolvers {
public:
  explicit query_continuous(CombineContext *context) :
      Resolvers("continuous", context) {};

  JsonDocument resolve(JsonObject reqData) override {
    ContinuousSchema schema = context->continuous->model->get();
    ContinuousSchema::Continuous continuous = schema.continuous;

    DynamicJsonDocument data(1024);
    data["isActive"] = continuous.isActive;
    JsonObject control = data.createNestedObject("control");
    control["type"] = ContinuousControlEnumToString(continuous.control.type);
    JsonArray channelOrders = control.createNestedArray("channelOrders");
    for (uint8_t channelOrder : continuous.control.channelOrders) {
      channelOrders.add(channelOrder);
    }

    JsonArray jarrPreconditions = data.createNestedArray("preconditions");
    for (int i = 0; i < sizeof(continuous.preconditions) / sizeof(continuous.preconditions[0]); i++) {
      ContinuousSchema::Continuous::Precondition precondition = continuous.preconditions[i];
      if (precondition.type != PREC_NONE) {
        JsonObject joPrecondition = jarrPreconditions.createNestedObject();
        joPrecondition["type"] = PreconditionEnumToString(precondition.type);
        joPrecondition["value"] = precondition.value;
      }
    }

    return data;
  }
};


//TODO: complete this
class query_continuous_state : public Resolvers {
public:
  explicit query_continuous_state(CombineContext *context) :
      Resolvers("channel_state", context, new permission_continuous_state(context)) {};

  JsonDocument resolve(JsonObject reqData) override {
    DynamicJsonDocument data(64);
    return data;
  }
};

//class mutation_channel_activate : public Resolvers {
//public:
//  explicit mutation_channel_activate(CombineContext *context) :
//      Resolvers(
//          "channel_activate",
//          context,
//          new permission_channel_activate(context)) {};
//
//  JsonDocument resolve(JsonObject reqData) override {
//    int index = reqData["index"];
//    boolean isActive = reqData["isActive"];
//    ChannelSchema channelSchema = context->channel->model->get();
//    channelSchema.channels[index].isActive = isActive;
//
//    context->channel->model->save(channelSchema);
//    delay(10);
//
//    ChannelSchema newChannelSchema = context->channel->model->get();
//    DynamicJsonDocument data(1024);
//    data["index"] = index;
//    data["isActive"] = newChannelSchema.channels[index].isActive;
//
//    // index = channel 0 - 7
//    context->channel->core->checkAndActivateControlType(newChannelSchema.channels[index], index);
//
//    return data;
//  }
//};
//
//class mutation_channel_save : public Resolvers {
//public:
//  explicit mutation_channel_save(CombineContext *context) :
//      Resolvers(
//          "channel_save", context,
//          new permission_channel_save(context)) {};
//
//  JsonDocument resolve(JsonObject reqData) override {
//    ChannelSchema channelSchema = context->channel->model->get();
//    int index = reqData["index"];
//
//    channelSchema.channels[index].control.type = ControlStringToEnum(reqData["control"]["type"]);
//    channelSchema.channels[index].control.value = reqData["control"]["value"];
//
//    // set precondition to providing args
//    for (int i = 0; i < reqData["preconditions"].size(); i++) {
//      channelSchema.channels[index].preconditions[i].type = PreconditionStringToEnum(
//          reqData["preconditions"][i]["type"]);
//      channelSchema.channels[index].preconditions[i].value = reqData["preconditions"][i]["value"];
//    }
//
//    // set precondition to none if providing args size < 3
//    for (int i = reqData["preconditions"].size(); i < 3; i++) {
//      channelSchema.channels[index].preconditions[i].type = PREC_NONE;
//      channelSchema.channels[index].preconditions[i].value = 0;
//    }
//
//    channelSchema.channels[index].isActive = false;
//    context->channel->core->checkAndActivateControlType(channelSchema.channels[index], index);
//    context->channel->model->save(channelSchema);
//    delay(10);
//
//    ChannelSchema newChannelSchema = context->channel->model->get();
//    ChannelSchema::Channel channel = newChannelSchema.channels[index];
//
//    DynamicJsonDocument data(1024);
//    data["index"] = index;
//    data["isActive"] = channel.isActive;
//    JsonObject control = data.createNestedObject("control");
//    control["type"] = ControlEnumToString(channel.control.type);
//    control["value"] = channel.control.value;
//
//    JsonArray jarrPreconditions = data.createNestedArray("preconditions");
//    for (int i = 0; i < sizeof(channel.preconditions) / sizeof(channel.preconditions[0]); i++) {
//      ChannelSchema::Channel::Precondition precondition = channel.preconditions[i];
//      if (precondition.type != PREC_NONE) {
//        JsonObject joPrecondition = jarrPreconditions.createNestedObject();
//        joPrecondition["type"] = PreconditionEnumToString(precondition.type);
//        joPrecondition["value"] = precondition.value;
//      }
//    }
//    return data;
//  }
//};

#endif //SG_MCU_CONTINUOUS_RESOLVERS_H
