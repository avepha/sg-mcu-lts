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

    if (reqData["mode"].isNull()) {
      Log::info("mutation", "if timer.mode field is not specified, set mode = 0");
    }

    bool isModeSpecified = false;
    ChannelTimerModeEnum mode;
    if (!reqData["mode"].isNull()) {
      if (!reqData["mode"].is<String>()) {
        InvalidInputError err("mode must be a string");
        throw err;
      }

      mode = ChannelTimerModeStringToEnum(reqData["mode"]);
      if (mode == CH_TIMER_NONE) {
        InvalidInputError err("Invalid timer mode");
        throw err;
      }

      isModeSpecified = true;
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

    if (reqData["timers"].size() > 20) {
      InvalidInputError err("timers size exceeded");
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

      if (isModeSpecified) {
        if (mode == CH_TIMER_INTERVAL) {
          if (timers[i]["stop"].isNull()) {
            InvalidInputError err("timers.stop must not be null");
            throw err;
          }
          if (!timers[i]["stop"].is<int>()) {
            InvalidInputError err("timers.stop must be an integer");
          }
        }
        else if (mode == CH_TIMER_WORKING) {
          if (timers[i]["working"].isNull()) {
            InvalidInputError err("timers.working must not be null");
            throw err;
          }
          if (!timers[i]["working"].is<int>()) {
            InvalidInputError err("timers.working must be an integer");
          }
        }
      }
      else {
        // mode default = CH_TIMER_INTERVAL
        if (timers[i]["stop"].isNull()) {
          InvalidInputError err("timers.stop or timers.working must not be null");
          throw err;
        }
        if (!timers[i]["stop"].is<int>()) {
          InvalidInputError err("timers.stop must be an integer");
        }
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
