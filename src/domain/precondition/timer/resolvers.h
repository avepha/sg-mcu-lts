#include "validationError.h"
#include "combineContext.h"

#include "util/util.h"

#include "./permission.h"

#ifndef SG_MCU_PRECONDITION_TIMER_RESOLVERS_H
#define SG_MCU_PRECONDITION_TIMER_RESOLVERS_H

// @mutation
class mutation_precondition_timer_save : public Mutation {
public:
  explicit mutation_precondition_timer_save() : Mutation("precondition_timer_save", new permission_precondition_timer_save()) {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    int index = reqData["index"];
    PrecTimerSchema schema = context->precTimer->model->get();

    schema.timers[index].size = reqData["timers"].size();
    for (int i = 0 ; i < schema.timers[index].size; i++) {
      schema.timers[index].timePair[i].start = reqData["timers"][i]["start"];
      schema.timers[index].timePair[i].stop = reqData["timers"][i]["stop"];
    }

    int writeOps = context->precTimer->model->save(schema);
    delay(10);

    PrecTimerSchema newSchema = context->precTimer->model->get();
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

class query_precondition_timer : public Query {
public:
  explicit query_precondition_timer() : Query("precondition_timer", new permission_precondition_timer()) {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    int index = reqData["index"];
    PrecTimerSchema newSchema = context->precTimer->model->get();

    DynamicJsonDocument data(1024);
    data["index"] = index;
    JsonArray timers = data.createNestedArray("timers");
    for (int i = 0; i < newSchema.timers[index].size; i++) {
      JsonObject jo = timers.createNestedObject();
      jo["start"] = newSchema.timers[index].timePair[i].start;
      jo["stop"] = newSchema.timers[index].timePair[i].stop;
    }

    return data;
  };
};

#endif //SG_MCU_RESOLVERS_H
