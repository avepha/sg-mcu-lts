#include "domain/permission.h"
#include "validationError.h"

#ifndef SG_MCU_TIMER_PERMISSION_H
#define SG_MCU_TIMER_PERMISSION_H

class permission_timer_save : public Permission {
public:
  explicit permission_timer_save() : Permission() {};

  void resolve(JsonObject reqData, CombineContext *context) override {
    if (reqData.isNull() || reqData["index"].isNull()) {
      InvalidInputError err("index must not be null");
      throw err;
    }

    if (reqData["index"] < 0 || reqData["index"] > 7) {
      InvalidInputError err("index out of range.");
      throw err;
    }

    if (reqData["timers"].isNull()) {
      InvalidInputError err("timers must not be null");
      throw err;
    }

    if (!reqData["timers"].is<JsonArray>()) {
      InvalidInputError err("timers must be an array");
      throw err;
    }

    if (reqData["timers"].size() <= 0) {
      InvalidInputError err("timers must not be an empty array");
      throw err;
    }

    JsonArray timers = reqData["timers"];
    for (int i = 0 ; i < timers.size() ; i++) {
      if (timers[i]["start"].isNull()) {
        InvalidInputError err("timers.start must not be null");
        throw err;
      }

      if (!timers[i]["start"].is<int>()) {
        InvalidInputError err("timers.start must not be a number");
        throw err;
      }

      if (timers[i]["stop"].isNull()) {
        InvalidInputError err("timers.stop must not be null");
        throw err;
      }

      if (!timers[i]["stop"].is<int>()) {
        InvalidInputError err("timers.stop must not be a number");
        throw err;
      }
    }
  }
};

class permission_timer : public Permission {
public:
  explicit permission_timer() : Permission() {};

  void resolve(JsonObject reqData, CombineContext *context) override {
    if (reqData.isNull() || reqData["index"].isNull()) {
      InvalidInputError err("index must not be null");
      throw err;
    }

    if (reqData["index"] < 0 || reqData["index"] > 7) {
      InvalidInputError err("index out of range.");
      throw err;
    }
  }
};

#endif //SG_MCU_PERMISSION_H
