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

class mutation_continuous_save : public Resolvers {
public:
  explicit mutation_continuous_save(CombineContext *context) :
      Resolvers(
          "continuous_save", context,
          new permission_continuous_save(context)) {};

  JsonDocument resolve(JsonObject reqData) override {
    ContinuousSchema schema = context->continuous->model->get();

    schema.continuous.control.type = ContinuousControlStringToEnum(reqData["control"]["type"]);
    for(int i = 0 ; i < reqData["control"]["channelOrders"].size(); i++) {
      schema.continuous.control.channelOrders[i] =  reqData["control"]["channelOrders"][i];
    }

    // set precondition by providing args
    for (int i = 0; i < reqData["preconditions"].size(); i++) {
      schema.continuous.preconditions[i].type = PreconditionStringToEnum(reqData["preconditions"][i]["type"]);
      schema.continuous.preconditions[i].value = reqData["preconditions"][i]["value"];
    }

    // set precondition to none if providing args size < 3
    for (int i = reqData["preconditions"].size(); i < 3; i++) {
      schema.continuous.preconditions[i].type = PREC_NONE;
      schema.continuous.preconditions[i].value = 0;
    }

    schema.continuous.isActive = false;
//    context->channel->core->checkAndActivateControlType(schema.continuous, index);
    context->continuous->model->save(schema);
    delay(10);

    ContinuousSchema newSchema = context->continuous->model->get();
    ContinuousSchema::Continuous continuous = newSchema.continuous;

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

#endif //SG_MCU_CONTINUOUS_RESOLVERS_H
