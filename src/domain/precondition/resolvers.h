#include "validationError.h"
#include "combineContext.h"
#include "domain/resolvers.h"
#include "util/util.h"

#ifndef SG_MCU_PRECONDITION_RESOLVERS_H
#define SG_MCU_PRECONDITION_RESOLVERS_H

// @query:
class query_precondition : public Resolvers {
public:
  explicit query_precondition(CombineContext *context) : Resolvers("precondition", context) {};

  String resolve(JsonObject json) override {
    if (json["data"]["type"].isNull() || json["data"]["index"].isNull()) {
      InvalidInputError err("type Or index field is not specified.");
      return err.toJsonString();
    }

    if (json["data"]["index"].as<int>() < 0 || json["data"]["index"].as<int>() > 7) {
      InvalidInputError err("index out of range.");
      return err.toJsonString();
    }

    int index = json["data"]["index"].as<int>();
    String type = json["data"]["type"].as<String>();

    if (type == "timer") {
      PreConditionTimerSchema timerSchema = context->preConditionContext->timerModel->get();
      StaticJsonDocument<1024> data;
      data["index"] = index;
      JsonArray timers = data.createNestedArray("timers");
      for (int i = 0; i < timerSchema.timer_size[index]; i++) {
        JsonArray d_i = timers.createNestedArray();
        d_i.add(timerSchema.timer[index][i][0]);
        d_i.add(timerSchema.timer[index][i][1]);
      }

      JsonTopic response(json["topic"], json["method"], data.as<JsonObject>());
      return response.toString();
    }

    if (type == "criteria") {

    }

    InvalidInputError err("No such the provided type.");
    return err.toJsonString();
  };
};


#endif //SG_MCU_RESOLVERS_H
