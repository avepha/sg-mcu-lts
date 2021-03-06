#include "validationError.h"
#include "combineContext.h"

#include "util/util.h"
#include "./permission.h"
#include "domain/channel-control/util/resolveControlEnum.h"
#include "domain/precondition/util/resolvePreconditionEnum.h"

#ifndef SG_MCU_CHANNEL_RESOLVERS_H
#define SG_MCU_CHANNEL_RESOLVERS_H

// @query
class query_channel : public Query {
public:
  explicit query_channel() : Query("channel", new permission_channel()) {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    int index = reqData["index"];
    ChannelSchema channelSchema = context->channel->model->get();
    ChannelSchema::Channel channel = channelSchema.channels[index];

    DynamicJsonDocument data(1024);
    data["index"] = index;
    data["isActive"] = channel.isActive;
    JsonObject control = data.createNestedObject("control");
    control["type"] = ChannelControlEnumToString(channel.control.type);
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

class query_channel_state : public Query {
public:
  explicit query_channel_state() : Query("channel_state", new permission_channel_state()) {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    int index = reqData["index"];
    Control *channelControl = context->channel->core->getControlByChannel(index);
    CONTROL_TYPE_ENUM type = channelControl->getType();

    DynamicJsonDocument data(1024);
    data["index"] = index;
    JsonArray chainOfPreconditions = data.createNestedArray("preconditions");

    switch (type) {
      case CH_CTRL_CRITERIA: {
        auto *ctrlCore = (CriteriaCore *) (channelControl);
        for (int i = 0; i < ctrlCore->getPreconditionSize(); i++) {
          if (ctrlCore->getPreconditionAt(i)->getType() == PREC_CRITERIA) {
            auto *precCoreAtN = (PrecCriteriaCore *) (ctrlCore->getPreconditionAt(i));
            chainOfPreconditions.add(precCoreAtN->getPreconditionState().report());
          }
          else if (ctrlCore->getPreconditionAt(i)->getType() == PREC_TIMER) {
            auto *precCoreAtN = (PrecTimerCore *) (ctrlCore->getPreconditionAt(i));
            chainOfPreconditions.add(precCoreAtN->getPreconditionState().report());
          }
          else if (ctrlCore->getPreconditionAt(i)->getType() == PREC_RANGE) {
            auto *precCoreAtN = (PrecRangeCore *) (ctrlCore->getPreconditionAt(i));
            chainOfPreconditions.add(precCoreAtN->getPreconditionState().report());
          }
        }

        data["control"] = ctrlCore->getControlState().report();
        break;
      }
      case CH_CTRL_RANGE: {
        auto *ctrlCore = (RangeCore *) (channelControl);
        for (int i = 0; i < ctrlCore->getPreconditionSize(); i++) {
          if (ctrlCore->getPreconditionAt(i)->getType() == PREC_CRITERIA) {
            auto *precCoreAtN = (PrecCriteriaCore *) (ctrlCore->getPreconditionAt(i));
            chainOfPreconditions.add(precCoreAtN->getPreconditionState().report());
          }
          else if (ctrlCore->getPreconditionAt(i)->getType() == PREC_TIMER) {
            auto *precCoreAtN = (PrecTimerCore *) (ctrlCore->getPreconditionAt(i));
            chainOfPreconditions.add(precCoreAtN->getPreconditionState().report());
          }
          else if (ctrlCore->getPreconditionAt(i)->getType() == PREC_RANGE) {
            auto *precCoreAtN = (PrecRangeCore *) (ctrlCore->getPreconditionAt(i));
            chainOfPreconditions.add(precCoreAtN->getPreconditionState().report());
          }
        }
        data["control"] = ctrlCore->getControlState().report();
        break;
      }
      case CH_CTRL_PAR: {
        auto *ctrlCore = (ParCore *) (channelControl);
        for (int i = 0; i < ctrlCore->getPreconditionSize(); i++) {
          if (ctrlCore->getPreconditionAt(i)->getType() == PREC_CRITERIA) {
            auto *precCoreAtN = (PrecCriteriaCore *) (ctrlCore->getPreconditionAt(i));
            chainOfPreconditions.add(precCoreAtN->getPreconditionState().report());
          }
          else if (ctrlCore->getPreconditionAt(i)->getType() == PREC_TIMER) {
            auto *precCoreAtN = (PrecTimerCore *) (ctrlCore->getPreconditionAt(i));
            chainOfPreconditions.add(precCoreAtN->getPreconditionState().report());
          }
          else if (ctrlCore->getPreconditionAt(i)->getType() == PREC_RANGE) {
            auto *precCoreAtN = (PrecRangeCore *) (ctrlCore->getPreconditionAt(i));
            chainOfPreconditions.add(precCoreAtN->getPreconditionState().report());
          }
        }
        data["control"] = ctrlCore->getControlState().report();
        break;
      }
      case CH_CTRL_TIMER: {
        auto *ctrlCore = (TimerCore *) (channelControl);
        data["control"] = ctrlCore->getControlState().report();
        break;
      }
      default: {
        break;
      }
    }

    return data;
  }
};

class mutation_channel_activate : public Mutation {
public:
  explicit mutation_channel_activate() : Mutation("channel_activate", new permission_channel_activate()) {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    int index = reqData["index"];
    boolean isActive = reqData["isActive"];
    ChannelSchema channelSchema = context->channel->model->get();
    channelSchema.channels[index].isActive = isActive;

    int writeOps = context->channel->model->save(channelSchema);
    delay(10);

    ChannelSchema newChannelSchema = context->channel->model->get();
    DynamicJsonDocument data(1024);
    data["writeOps"] = writeOps;
    data["index"] = index;
    data["isActive"] = newChannelSchema.channels[index].isActive;

    // index = channel 0 - 7
    context->channel->core->checkAndActivateControl(newChannelSchema.channels[index], index);

    return data;
  }
};

class mutation_channel_save : public Mutation {
public:
  explicit mutation_channel_save() : Mutation("channel_save", new permission_channel_save()) {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    ChannelSchema channelSchema = context->channel->model->get();
    int index = reqData["index"];

    channelSchema.channels[index].control.type = ChannelControlStringToEnum(reqData["control"]["type"]);
    channelSchema.channels[index].control.value = reqData["control"]["value"];

    // set precondition to providing args
    if (!reqData["preconditions"].isNull()) {
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
    }

    channelSchema.channels[index].isActive = false;
    context->channel->core->checkAndActivateControl(channelSchema.channels[index], index);
    int writeOps = context->channel->model->save(channelSchema);
    delay(10);

    ChannelSchema newChannelSchema = context->channel->model->get();
    ChannelSchema::Channel channel = newChannelSchema.channels[index];

    DynamicJsonDocument data(1024);
    data["writeOps"] = writeOps;
    data["index"] = index;
    data["isActive"] = channel.isActive;
    JsonObject control = data.createNestedObject("control");
    control["type"] = ChannelControlEnumToString(channel.control.type);
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

#endif //SG_MCU_RESOLVERS_H
