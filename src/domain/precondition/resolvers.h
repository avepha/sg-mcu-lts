#include "validationError.h"
#include "combineContext.h"
#include "domain/resolvers.h"
#include "util/util.h"

#ifndef SG_MCU_PRECONDITION_RESOLVERS_H
#define SG_MCU_PRECONDITION_RESOLVERS_H

// @mutation
class mutation_precondition_save : public Resolvers {
public:
  explicit mutation_precondition_save(CombineContext *context) : Resolvers("precondition_save", context) {};

  String resolve(JsonObject json) override {
    if (json["data"]["index"].isNull() || json["data"]["timers"].isNull()) {
      InvalidInputError err("index or timers field is not specified.");
      return err.toJsonString();
    }

    if (json["data"]["index"].as<int>() < 0 || json["data"]["index"].as<int>() > 7) {
      InvalidInputError err("index out of range.");
      return err.toJsonString();
    }

    int index = json["data"]["index"].as<int>();

    if (!json["data"]["timers"].isNull()) {
      PreConditionTimerSchema timerSchema = context->preConditionContext->timerModel->get();
      int size = json["data"]["timers"].size();

      timerSchema.timer_size[index] = size;
      for(int i = 0 ; i < size; i++) {
        timerSchema.timer[index][i][0] = json["data"]["timers"][i][0];
        timerSchema.timer[index][i][1] = json["data"]["timers"][i][1];
      }

      int writeOps = context->preConditionContext->timerModel->save(timerSchema);
      delay(10);

      PreConditionTimerSchema newSchema = context->preConditionContext->timerModel->get();
      StaticJsonDocument<1024> data;
      data["index"] = index;
      data["writeOps"] = writeOps;
      JsonArray timers = data.createNestedArray("timers");
      for (int i = 0; i < newSchema.timer_size[index]; i++) {
        JsonArray d_i = timers.createNestedArray();
        d_i.add(newSchema.timer[index][i][0]);
        d_i.add(newSchema.timer[index][i][1]);
      }

      JsonTopic response(json["topic"], json["method"], data.as<JsonObject>());
      return response.toString();
    }

    InvalidInputError err("No such the provided type.");
    return err.toJsonString();
  };
};

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
