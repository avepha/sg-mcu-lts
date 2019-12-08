#include "domain/permission.h"
#include "validationError.h"
#include "domain/sequence-control/util/resolveSequenceControlEnum.h"
#include "domain/precondition/util/resolvePreconditionEnum.h"

#ifndef SG_MCU_SEQUENCE_PERMISSION_H
#define SG_MCU_SEQUENCE_PERMISSION_H

class permission_sequence_state : public Permission {
public:
  explicit permission_sequence_state() : Permission() {};

  void resolve(JsonObject reqData, CombineContext *context) override {
    SequenceSchema schema = context->sequence->model->get();
    if (!schema.sequence.isActive) {
      InactiveControlError err;
      throw err;
    }
  }
};

class permission_sequence_save : public Permission {
public:
  explicit permission_sequence_save() : Permission() {};

  void resolve(JsonObject reqData, CombineContext *context) override {
    if (reqData["control"].isNull() || reqData["control"]["type"].isNull()) {
      InvalidInputError err("control field must have type.");
      throw err;
    }

    SEQUENCE_CONTROL_TYPE_ENUM type = SequenceControlStringToEnum(reqData["control"]["type"]);
    if (type == SEQ_CTRL_UNKNOWN) {
      InvalidInputError err("Unknown sequence control");
      throw err;
    }

    if (!reqData["preconditions"].isNull()) {
      if (!reqData["preconditions"].is<JsonArray>()) {
        InvalidInputError err("preconditions field must be an array.");
        throw err;
      }

      if (type == SEQ_CTRL_TIMER && reqData["preconditions"].size() > 0) {
        InvalidInputError err("preconditions field must be an empty array when applying sequence-timer");
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

class permission_sequence_order_save : public Permission {
public:
  explicit permission_sequence_order_save() : Permission() {};

  void resolve(JsonObject reqData, CombineContext *context) override {
    if (reqData["channelOrderAndTiming"].isNull() || !reqData["channelOrderAndTiming"].is<JsonArray>()) {
      InvalidInputError err("channelOrderAndTiming field must be an array");
      throw err;
    }

    if (reqData["channelOrderAndTiming"].size() > 8 ) {
      IndexOutOfBoundError err("channelOrderAndTiming index out of bound");
      throw err;
    }

    if (reqData["channelOrderAndTiming"].size() == 0 ) {
      InvalidInputError err("channelOrderAndTiming field must not be empty array");
      throw err;
    }

    JsonArray channelOrderAndTiming = reqData["channelOrderAndTiming"];
    for (int i = 0 ; i < channelOrderAndTiming.size(); i++) {
      if (channelOrderAndTiming[i]["channel"].isNull() || !channelOrderAndTiming[i]["channel"].is<int>()) {
        InvalidInputError err("channel field must be an integer.");
        throw err;
      }
      if (channelOrderAndTiming[i]["working"].isNull() || !channelOrderAndTiming[i]["working"].is<int>()) {
        InvalidInputError err("working field must be an integer.");
        throw err;
      }
    }
  }
};

class permission_sequence_activate : public Permission {
public:
  explicit permission_sequence_activate() : Permission() {};

  void resolve(JsonObject reqData, CombineContext *context) override {
    ControlSchema controlSchema = context->control->model->get();
    if (controlSchema.type != CTRL_SEQUENCE) {
      InactiveSequenceControlError err;
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

#endif //SG_MCU_SEQUENCE_PERMISSION_H
