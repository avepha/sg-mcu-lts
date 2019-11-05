#include "domain/permission.h"
#include "validationError.h"
#include "domain/continuous-control/util/resolveContinuousControlEnum.h"
#include "domain/precondition/util/resolvePreconditionEnum.h"

#ifndef SG_MCU_CONTINUOUS_PERMISSION_H
#define SG_MCU_CONTINUOUS_PERMISSION_H

class permission_continuous_state : public Permission {
public:
  explicit permission_continuous_state() : Permission() {};

  void resolve(JsonObject reqData, CombineContext *context) override {
    ContinuousSchema schema = context->continuous->model->get();
    if (!schema.continuous.isActive) {
      InactiveControlError err;
      throw err;
    }
  }
};

class permission_continuous_save : public Permission {
public:
  explicit permission_continuous_save() : Permission() {};

  void resolve(JsonObject reqData, CombineContext *context) override {
    if (reqData["control"].isNull() || reqData["control"]["type"].isNull()) {
      InvalidInputError err("control field must have type.");
      throw err;
    }

    CONTINUOUS_CONTROL_TYPE_ENUM type = ContinuousControlStringToEnum(reqData["control"]["type"]);
    if (type == CON_CTRL_UNKNOWN) {
      InvalidInputError err("Unknown continuous control");
      throw err;
    }

    if (reqData["control"]["channelOrderAndTiming"].isNull() || !reqData["control"]["channelOrderAndTiming"].is<JsonArray>()) {
      InvalidInputError err("channelOrders field must be an array");
      throw err;
    }

    if (reqData["control"]["channelOrderAndTiming"].size() > 8 ) {
      IndexOutOfBoundError err("channelOrderAndTiming index out of bound");
      throw err;
    }

    JsonArray channelOrderAndTiming = reqData["control"]["channelOrderAndTiming"];
    for (int i = 0 ; i < channelOrderAndTiming.size(); i++) {
      if (channelOrderAndTiming[i]["channel"].isNull() || !channelOrderAndTiming[i]["channel"].is<int>()) {
        InvalidInputError err("channel field must be an integer.");
        throw err;
      }
      if (channelOrderAndTiming[i]["workingTimeInSec"].isNull() || !channelOrderAndTiming[i]["workingTimeInSec"].is<int>()) {
        InvalidInputError err("workingTimeInSec field must be an integer.");
        throw err;
      }
    }

    if (reqData["preconditions"].isNull() || !reqData["preconditions"].is<JsonArray>()) {
      InvalidInputError err("preconditions field must be an array.");
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
};

class permission_continuous_activate : public Permission {
public:
  explicit permission_continuous_activate() : Permission() {};

  void resolve(JsonObject reqData, CombineContext *context) override {
    ControlSchema controlSchema = context->control->model->get();
    if (controlSchema.type != CTRL_CONTINUOUS) {
      InactiveContinuousControlError err;
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

#endif //SG_MCU_CONTINUOUS_PERMISSION_H
