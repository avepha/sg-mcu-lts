#include "util/util.h"


#include "validationError.h"
#include "combineContext.h"

#include "./permission.h"

#ifndef SG_MCU_TIMER_RESOLVERS_H
#define SG_MCU_TIMER_RESOLVERS_H

// @mutation
class mutation_timer_save : public Mutation {
public:
  explicit mutation_timer_save() : Mutation("timer_save", new permission_timer_save) {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    int index = reqData["index"];
    TimerSchema schema = context->timer->model->get();
    int size = reqData["timers"].size();

    schema.timers[index].size = size;
    schema.timers[index].size = reqData["timers"].size();
    for (int i = 0 ; i < schema.timers[index].size; i++) {
      schema.timers[index].timePair[i].start = reqData["timers"][i]["start"];
      schema.timers[index].timePair[i].stop = reqData["timers"][i]["stop"];
    }

    int writeOps = context->timer->model->save(schema);
    TimerSchema newSchema = context->timer->model->get();

    DynamicJsonDocument data(1024);
    data["index"] = index;
    data["writeOps"] = writeOps;
    JsonArray timers = data.createNestedArray("timers");
    for (int i = 0; i < newSchema.timers[index].size; i++) {
      JsonObject jo = timers.createNestedObject();
      jo["start"] = newSchema.timers[index].timePair[i].start;
      jo["stop"] = newSchema.timers[index].timePair[i].stop;
    }

    return data;
  };
};

// @query: date
class query_timer : public Query {
public:
  explicit query_timer() : Query("timer", new permission_timer) {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    int index = reqData["index"];
    TimerSchema schema = context->timer->model->get();
    DynamicJsonDocument data(1024);
    data["index"] = index;
    JsonArray timers = data.createNestedArray("timers");
    for (int i = 0; i < schema.timers[index].size; i++) {
      JsonObject jo = timers.createNestedObject();
      jo["start"] = schema.timers[index].timePair[i].start;
      jo["stop"] = schema.timers[index].timePair[i].stop;
    }

    return data;
  };
};

#endif //SG_MCU_RESOLVERS_H
