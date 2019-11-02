#include "domain/permission.h"
#include "validationError.h"
#include "domain/channel-control/util/resolveControlEnum.h"
#include "domain/precondition/util/resolvePreconditionEnum.h"

#ifndef SG_MCU_CHANNEL_PERMISSION_H
#define SG_MCU_CHANNEL_PERMISSION_H

class permission_channel : public Permission {
public:
  explicit permission_channel(CombineContext *context) : Permission(context) {};

  void resolve(JsonObject reqData) override {
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
  explicit permission_channel_state(CombineContext *context) : Permission(context) {};

  void resolve(JsonObject reqData) override {
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
  }
};

class permission_channel_save : public Permission {
public:
  explicit permission_channel_save(CombineContext *context) : Permission(context) {};

  void resolve(JsonObject reqData) override {
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

    if (ControlStringToEnum(reqData["control"]["type"]) == CTRL_UNKNOWN) {
      InvalidInputError err("Unknown control type");
      throw err;
    }

    if (reqData["preconditions"].isNull() || !reqData["preconditions"].is<JsonArray>()) {
      InvalidInputError err("preconditions field must be an array.");
      throw err;
    }

    if (reqData["preconditions"].size() > 3) {
      InvalidInputError err("preconditions field must have maximum length of 3");
      throw err;
    }

    for (int i = 0 ; i < reqData["preconditions"].as<JsonArray>().size(); i++) {
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
};

class permission_channel_activate : public Permission {
public:
  explicit permission_channel_activate(CombineContext *context) : Permission(context) {};

  void resolve(JsonObject reqData) override {
    if (reqData["index"].isNull()) {
      IndexNotSpecifyError err;
      throw err;
    }

    if (reqData["index"] < 0 || reqData["index"] > 7) {
      IndexOutOfBoundError err;
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
