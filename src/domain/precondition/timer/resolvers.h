#include "validationError.h"
#include "combineContext.h"
#include "domain/resolvers.h"
#include "util/util.h"

#include "./permission.h"

#ifndef SG_MCU_PRECONDITION_TIMER_RESOLVERS_H
#define SG_MCU_PRECONDITION_TIMER_RESOLVERS_H

// @mutation
class mutation_precondition_timer_save : public Resolvers {
public:
  explicit mutation_precondition_timer_save(CombineContext *context) :
      Resolvers("precondition_timer_save", context, new permission_precondition_timer_save(context)) {};

  JsonDocument resolve(JsonObject reqData) override {
    int index = reqData["index"];
    PrecTimerSchema schema = context->precTimer->model->get();

    schema.timers[index].size = reqData["timers"].size();
    for (int i = 0 ; i < schema.timers[index].size; i++) {
      schema.timers[index].data[i][0] = reqData["timers"][i][0];
      schema.timers[index].data[i][1] = reqData["timers"][i][1];
    }

    int writeOps = context->precTimer->model->save(schema);
    delay(10);

    PrecTimerSchema newSchema = context->precTimer->model->get();
    DynamicJsonDocument data(1024);
    data["index"] = index;
    data["writeOps"] = writeOps;
    JsonArray timers = data.createNestedArray("timers");
    for (int i = 0; i < newSchema.timers[index].size; i++) {
      JsonArray d_i = timers.createNestedArray();
      d_i.add(newSchema.timers[index].data[i][0]);
      d_i.add(newSchema.timers[index].data[i][1]);
    }

    return data;
  };
};

class query_precondition_timer : public Resolvers {
public:
  explicit query_precondition_timer(CombineContext *context) :
      Resolvers("precondition_timer", context, new permission_precondition_timer(context)) {};

  JsonDocument resolve(JsonObject reqData) override {
    int index = reqData["index"];
    PrecTimerSchema newSchema = context->precTimer->model->get();

    DynamicJsonDocument data(1024);
    data["index"] = index;
    JsonArray timers = data.createNestedArray("timers");
    for (int i = 0; i < newSchema.timers[index].size; i++) {
      JsonArray d_i = timers.createNestedArray();
      d_i.add(newSchema.timers[index].data[i][0]);
      d_i.add(newSchema.timers[index].data[i][1]);
    }

    return data;
  };
};

#endif //SG_MCU_RESOLVERS_H
