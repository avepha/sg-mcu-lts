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

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    SequenceTimerSchema schema = context->sequenceTimer->model->get();
    for (int i = 0 ; i < reqData["timers"].size(); i++) {
      schema.timer.timePair[i].start = reqData["timers"][i]["start"];
      schema.timer.timePair[i].stop = reqData["timers"][i]["stop"];
    }
    schema.timer.size = reqData["timers"].size();

    int writeOps = context->sequenceTimer->model->save(schema);

    SequenceTimerSchema newSchema = context->sequenceTimer->model->get();
    DynamicJsonDocument data(1024);
    data["writeOps"] = writeOps;
    JsonArray timers = data.createNestedArray("timers");
    for (int i = 0; i < newSchema.timer.size; i++) {
      JsonObject jo = timers.createNestedObject();
      jo["start"] = newSchema.timer.timePair[i].start;
      jo["stop"] = newSchema.timer.timePair[i].stop;
    }

    return data;
  };
};

// @query: date
class query_sequence_timer : public Query {
public:
  explicit query_sequence_timer() : Query("sequence_timer") {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    SequenceTimerSchema schema = context->sequenceTimer->model->get();

    DynamicJsonDocument data(1024);
    JsonArray timers = data.createNestedArray("timers");
    for (int i = 0; i < schema.timer.size; i++) {
      JsonObject jo = timers.createNestedObject();
      jo["start"] = schema.timer.timePair[i].start;
      jo["stop"] = schema.timer.timePair[i].stop;
    }

    return data;
  };
};

#endif //SG_MCU_RESOLVERS_H
