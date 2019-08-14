#include "util/util.h"

#include "domain/resolvers.h"
#include "validationError.h"
#include "combineContext.h"

#ifndef SG_MCU_TIMER_RESOLVERS_H
#define SG_MCU_TIMER_RESOLVERS_H

// @mutation
class mutation_timer_save : public Resolvers {
public:
  explicit mutation_timer_save(CombineContext *context) : Resolvers("timer_save", context) {};

  String resolve(JsonObject reqJson) override {
    if (reqJson["data"].isNull() || reqJson["data"]["index"].isNull() || reqJson["data"]["timers"].isNull()) {
      InvalidInputError err("index or timers field is not specified.");
      return err.toJsonString();
    }

    if (reqJson["data"]["index"].as<int>() < 0 || reqJson["data"]["index"].as<int>() > 7) {
      InvalidInputError err("index out of range.");
      return err.toJsonString();
    }

    int index = reqJson["data"]["index"].as<int>();
    TimerSchema timerSchema = context->timerContext->model->get();
    int size = reqJson["data"]["timers"].size();

    timerSchema.timers[index].size = size;
    for(int i = 0 ; i < size; i++) {
      timerSchema.timers[index].data[i][0] = reqJson["data"]["timers"][i][0];
      timerSchema.timers[index].data[i][1] = reqJson["data"]["timers"][i][1];
    }

    int writeOps = context->timerContext->model->save(timerSchema);
    delay(10);

    TimerSchema newSchema = context->timerContext->model->get();
    StaticJsonDocument<1024> data;
    data["index"] = index;
    data["writeOps"] = writeOps;
    JsonArray timers = data.createNestedArray("timers");
    for (int i = 0; i < newSchema.timers[index].size; i++) {
      JsonArray d_i = timers.createNestedArray();
      d_i.add(newSchema.timers[index].data[i][0]);
      d_i.add(newSchema.timers[index].data[i][1]);
    }

    JsonTopic response(reqJson["topic"], reqJson["method"], data.as<JsonObject>());
    return response.toString();
  };
};

// @query: date
class query_timer : public Resolvers {
public:
  explicit query_timer(CombineContext *context) : Resolvers("timer", context) {};

  String resolve(JsonObject reqJson) override {
    if (reqJson["data"].isNull() || reqJson["data"]["index"].isNull()) {
      InvalidInputError error("index is not specified.");
      return error.toJsonString();
    }

    if (reqJson["data"]["index"] < 0 || reqJson["data"]["index"] > 7) {
      InvalidInputError error("index out of range.");
      return error.toJsonString();
    }

    int index = reqJson["data"]["index"];
    TimerSchema timerSchema = context->timerContext->model->get();
    StaticJsonDocument<1024> data;
    data["index"] = index;
    JsonArray timers = data.createNestedArray("timers");
    for (int i = 0; i < timerSchema.timers[index].size; i++) {
      JsonArray d_i = timers.createNestedArray();
      d_i.add(timerSchema.timers[index].data[i][0]);
      d_i.add(timerSchema.timers[index].data[i][1]);
    }

    JsonTopic response(reqJson["topic"], reqJson["method"], data.as<JsonObject>());
    return response.toString();
  };
};

#endif //SG_MCU_RESOLVERS_H
