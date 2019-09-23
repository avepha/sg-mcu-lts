#include "domain/permission.h"
#include "validationError.h"

#ifndef SG_MCU_CHANNEL_PERMISSION_H
#define SG_MCU_CHANNEL_PERMISSION_H

class permission_channel : public Permission {
public:
  explicit permission_channel(CombineContext *context) : Permission(context) {};

  void resolve(JsonObject reqData) override {
    if (reqData["index"].isNull()) {
      InvalidInputError err("index field is not specified.");
      throw err;
    }

    if (reqData["index"] < 0 || reqData["index"] > 7) {
      InvalidInputError err("index out of range.");
      throw err;
    }
  }
};


class permission_channel_save : public Permission {
public:
  explicit permission_channel_save(CombineContext *context) : Permission(context) {};

  void resolve(JsonObject reqData) override {
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
  }
};


#endif //SG_MCU_PERMISSION_H
