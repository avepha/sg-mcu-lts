#include "domain/permission.h"
#include "validationError.h"
#include "domain/channel-control/util/resolveControlEnum.h"
#include "domain/precondition/util/resolvePreconditionEnum.h"

#ifndef SG_MCU_CHANNEL_PERMISSION_H
#define SG_MCU_CHANNEL_PERMISSION_H

class permission_channel : public Permission {
public:
  explicit permission_channel() : Permission() {};

  void resolve(JsonObject reqData, CombineContext *context) override {
    if (reqData["index"].isNull()) {
      IndexNotSpecifyError err;
      throw err;
    }

    if (reqData["index"] < 0 || reqData["index"] > 7) {
      IndexOutOfBoundError err;
      throw err;
    }
  }
};

class permission_channel_state : public Permission {
public:
  explicit permission_channel_state() : Permission() {};

  void resolve(JsonObject reqData, CombineContext *context) override {
    ControlSchema controlSchema = context->control->model->get();
    if (controlSchema.type != CTRL_CHANNEL) {
      InactiveChannelControlError err;
      throw err;
    }

    if (reqData["index"].isNull()) {
      IndexNotSpecifyError err;
      throw err;
    }

    if (reqData["index"] < 0 || reqData["index"] > 7) {
      IndexOutOfBoundError err;
      throw err;
    }

    int index = reqData["index"];
    ChannelSchema schema = context->channel->model->get();
    if (!schema.channels[index].isActive) {
      InactiveControlError err;
      throw err;
    }

    Control *channelControl = context->channel->core->getControlByChannel(index);
    if (channelControl == nullptr) {
      NoControlStateError err;
      throw err;
    }
  }
};

class permission_channel_save : public Permission {
public:
  explicit permission_channel_save() : Permission() {};

  void resolve(JsonObject reqData, CombineContext *context) override {
    if (reqData["index"].isNull()) {
      IndexNotSpecifyError err;
      throw err;
    }

    if (reqData["index"] < 0 || reqData["index"] > 7) {
      IndexOutOfBoundError err;
      throw err;
    }

    if (reqData["control"].isNull() || reqData["control"]["type"].isNull() || reqData["control"]["value"].isNull()) {
      InvalidInputError err("control field must have type and value.");
      throw err;
    }
    CONTROL_TYPE_ENUM type = ChannelControlStringToEnum(reqData["control"]["type"]);
    if (type == CH_CTRL_UNKNOWN) {
      InvalidInputError err("Unknown control type");
      throw err;
    }

    if (!reqData["preconditions"].isNull()) {
      if (!reqData["preconditions"].is<JsonArray>()) {
        InvalidInputError err("preconditions field must be an array.");
        throw err;
      }

      if (type == CH_CTRL_TIMER && reqData["preconditions"].size() > 0) {
        InvalidInputError err("preconditions field must be an empty array when applying channel-timer");
        throw err;
      }

      if (reqData["preconditions"].size() > 3) {
        InvalidInputError err("preconditions field must have maximum length of 3");
        throw err;
      }

      for (int i = 0; i < reqData["preconditions"].as<JsonArray>().size(); i++) {
        JsonObject jo = reqData["preconditions"][i];
        if (jo["type"].isNull() || jo["value"].isNull()) {
          InvalidInputError err("precondition must have field type and value");
          throw err;
        }

        if (PreconditionStringToEnum(jo["type"]) == PREC_UNKNOWN) {
          InvalidInputError err("Unknown Precondition");
          throw err;
        }
      }
    }
  }
};

class permission_channel_activate : public Permission {
public:
  explicit permission_channel_activate() : Permission() {};

  void resolve(JsonObject reqData, CombineContext *context) override {
    ControlSchema controlSchema = context->control->model->get();
    if (controlSchema.type != CTRL_CHANNEL) {
      InactiveChannelControlError err;
      throw err;
    }

    if (reqData["isActive"].isNull()) {
      InvalidInputError err("data field must have isActive and value.");
      throw err;
    }

    if (!reqData["isActive"].is<boolean>()) {
      InvalidInputError err("isActive field must be boolean.");
      throw err;
    }
  }
};

#endif //SG_MCU_PERMISSION_H
