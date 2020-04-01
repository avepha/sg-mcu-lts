#include "domain/mutation.h"
#include "domain/query.h"
#include "util/util.h"

#include "validationError.h"
#include "combineContext.h"

#include "./permission.h"

#ifndef SG_MCU_SEQUENCE_RANGE_RESOLVERS_H
#define SG_MCU_SEQUENCE_RANGE_RESOLVERS_H

// @mutation
class mutation_sequence_range_save : public Mutation {
public:
  explicit mutation_sequence_range_save() : Mutation("sequence_range_save", new permission_sequence_range_save()) {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    JsonObject range = reqData["range"];
    SequenceRangeSchema schema = context->sequenceRange->model->get();
    schema.range.sensor = range["sensor"];
    schema.range.upper = range["upper"];
    schema.range.lower = range["lower"];
    schema.range.greater = range["greater"];

    if (!range["timing"].isNull()) {
      schema.range.timing.waitingTimeInSecond = range["timing"]["waiting"];
      schema.range.timing.enable = true;
    }
    else {
      schema.range.timing.enable = false;
    }

    int writeOps = context->sequenceRange->model->save(schema);
    SequenceRangeSchema newSchema = context->sequenceRange->model->get();
    DynamicJsonDocument data(1024);
    data["writeOps"] = writeOps;
    JsonObject newRange = data.createNestedObject("range");
    newRange["sensor"] = newSchema.range.sensor;
    newRange["upper"] = newSchema.range.upper;
    newRange["lower"] = newSchema.range.lower;
    newRange["greater"] = newSchema.range.greater;
    newRange["timing"]["enable"] = newSchema.range.timing.enable;
    newRange["timing"]["waiting"] = newSchema.range.timing.waitingTimeInSecond;
    return data;
  };
};

// @query: date
class query_sequence_range : public Query {
public:
  explicit query_sequence_range() : Query("sequence_range") {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    SequenceRangeSchema schema = context->sequenceRange->model->get();
    DynamicJsonDocument data(1024);
    JsonObject newRange = data.createNestedObject("range");
    newRange["sensor"] = schema.range.sensor;
    newRange["upper"] = schema.range.upper;
    newRange["lower"] = schema.range.lower;
    newRange["greater"] = schema.range.greater;
    newRange["timing"]["enable"] = schema.range.timing.enable;
    newRange["timing"]["waiting"] = schema.range.timing.waitingTimeInSecond;
    return data;
  };
};

#endif //SG_MCU_SEQUENCE_RANGE_RESOLVERS_H
