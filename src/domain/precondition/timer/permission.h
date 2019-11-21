#include "domain/permission.h"
#include "validationError.h"

#ifndef SG_MCU_PRECONDITION_TIMER_PERMISSION_H
#define SG_MCU_PRECONDITION_TIMER_PERMISSION_H

class permission_precondition_timer_save : public Permission {
public:
  explicit permission_precondition_timer_save() : Permission() {};

  void resolve(JsonObject reqData, CombineContext *context) override {
    if (reqData.isNull() || reqData["index"].isNull()) {
      InvalidInputError err("index is not specified.");
      throw err;
    }

    if (reqData["index"] < 0 || reqData["index"] > 7) {
      InvalidInputError err("index out of range.");
      throw err;
    }

    if (reqData["timers"].isNull()) {
      InvalidInputError err("timers field is not specified.");
      throw err;
    }

    if (!reqData["timers"].is<JsonArray>()) {
      InvalidInputError err("timers field must be an array.");
      throw err;
    }

    if (reqData["timers"].size() <= 0) {
      InvalidInputError err("timers field size must not be zero.");
      throw err;
    }

    if (reqData["timers"].size() > 20) {
      InvalidInputError err("timers has exceeded");
      throw err;
    }

    JsonArray timers = reqData["timers"];
    for (int i = 0; i < timers.size(); i++) {
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

class permission_precondition_timer : public Permission {
public:
  explicit permission_precondition_timer() : Permission() {};

  void resolve(JsonObject reqData, CombineContext *context) override {
    if (reqData.isNull() || reqData["index"].isNull()) {
      InvalidInputError err("index is not specified.");
      throw err;
    }

    if (reqData["index"] < 0 || reqData["index"] > 7) {
      InvalidInputError err("index out of range.");
      throw err;
    }
  }
};


#endif //SG_MCU_PERMISSION_H
