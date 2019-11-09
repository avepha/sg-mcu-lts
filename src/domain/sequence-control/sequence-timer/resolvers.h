#include "domain/mutation.h"
#include "domain/query.h"
#include "util/util.h"


#include "validationError.h"
#include "combineContext.h"

#include "./permission.h"
#ifndef SG_MCU_SEQUENCE_TIMER_RESOLVERS_H
#define SG_MCU_SEQUENCE_TIMER_RESOLVERS_H

// @mutation
class mutation_sequence_timer_save : public Mutation {
public:
  explicit mutation_sequence_timer_save() : Mutation("sequence_timer_save", new permission_sequence_timer_save) {};

  JsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    SequenceTimerSchema schema = context->sequenceTimer->model->get();
    for (int i = 0 ; i < reqData["timers"].size(); i++) {
      schema.timer.data[i][0] = reqData["timers"][i][0];
      schema.timer.data[i][1] = reqData["timers"][i][1];
    }
    schema.timer.size = reqData["timers"].size();

    int writeOps = context->sequenceTimer->model->save(schema);

    SequenceTimerSchema newSchema = context->sequenceTimer->model->get();
    DynamicJsonDocument data(1024);
    data["writeOps"] = writeOps;
    JsonArray timers = data.createNestedArray("timers");
    for (int i = 0 ; i < newSchema.timer.size; i++) {
      JsonArray jarr = timers.createNestedArray();
      jarr.add(newSchema.timer.data[i][0]);
      jarr.add(newSchema.timer.data[i][1]);
    }

    return data;
  };
};

// @query: date
class query_sequence_timer : public Query {
public:
  explicit query_sequence_timer() : Query("sequence_timer") {};

  JsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    SequenceTimerSchema schema = context->sequenceTimer->model->get();

    DynamicJsonDocument data(1024);
    JsonArray timers = data.createNestedArray("timers");
    for (int i = 0 ; i < schema.timer.size; i++) {
      JsonArray jarr = timers.createNestedArray();
      jarr.add(schema.timer.data[i][0]);
      jarr.add(schema.timer.data[i][1]);
    }

    return data;
  };
};

#endif //SG_MCU_RESOLVERS_H
