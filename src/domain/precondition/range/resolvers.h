#include "util/util.h"

#include "domain/resolvers.h"
#include "validationError.h"
#include "combineContext.h"

#include "./permission.h"

#ifndef SG_MCU_PRECONDITION_RANGE_RESOLVERS_H
#define SG_MCU_PRECONDITION_RANGE_RESOLVERS_H

// @mutation
class mutation_precondition_range_save : public Resolvers {
public:
  explicit mutation_precondition_range_save(CombineContext *context) : Resolvers("precondition_range_save", context, new permission_precondition_range_save(context)) {};

  JsonDocument resolve(JsonObject reqData) override {
    int index = reqData["index"];
    PrecRangeSchema schema = context->precRange->model->get();

    JsonObject range = reqData["range"];

    schema.ranges[index].sensor = range["sensor"];
    schema.ranges[index].upper = range["upper"];
    schema.ranges[index].lower = range["lower"];

    int writeOps = context->precRange->model->save(schema);

    PrecRangeSchema newSchema = context->precRange->model->get();

    DynamicJsonDocument data(256);
    data["index"] = index;
    data["writeOps"] = writeOps;
    JsonObject _range = data.createNestedObject("range");
    _range["sensor"] = newSchema.ranges[index].sensor;
    _range["upper"] = newSchema.ranges[index].upper;
    _range["lower"] = newSchema.ranges[index].lower;

    return data;
  };
};

// @query: date
class query_precondition_range : public Resolvers {
public:
  explicit query_precondition_range(CombineContext *context) : Resolvers("precondition_range", context, new permission_precondition_range(context)) {};

  JsonDocument resolve(JsonObject reqData) override {
    int index = reqData["index"];
    PrecRangeSchema schema = context->precRange->model->get();

    DynamicJsonDocument data(256);
    data["index"] = index;
    JsonObject range = data.createNestedObject("range");
    range["sensor"] = schema.ranges[index].sensor;
    range["upper"] = schema.ranges[index].upper;
    range["lower"] = schema.ranges[index].lower;

    return data;
  };
};

#endif //SG_MCU_RESOLVERS_H
