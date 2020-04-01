#include "util/util.h"


#include "validationError.h"
#include "combineContext.h"

#include "./permission.h"

#ifndef SG_MCU_RANGE_RESOLVERS_H
#define SG_MCU_RANGE_RESOLVERS_H

// @mutation
class mutation_range_save : public Mutation {
public:
  explicit mutation_range_save() : Mutation("range_save", new permission_range_save()) {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    int index = reqData["index"];
    RangeSchema schema = context->range->model->get();

    JsonObject range = reqData["range"];

    schema.ranges[index].sensor = range["sensor"];
    schema.ranges[index].upper = range["upper"];
    schema.ranges[index].lower = range["lower"];
    schema.ranges[index].greater = range["greater"];

    if (!range["timing"].isNull()) {
      schema.ranges[index].timing.enable = true;
      schema.ranges[index].timing.workingTimeInSecond = range["timing"]["working"];
      schema.ranges[index].timing.waitingTimeInSecond = range["timing"]["waiting"];
    }
    else {
      schema.ranges[index].timing.enable = false;
    }

    int writeOps = context->range->model->save(schema);

    RangeSchema newSchema = context->range->model->get();

    DynamicJsonDocument data(256);
    data["index"] = index;
    data["writeOps"] = writeOps;
    JsonObject _range = data.createNestedObject("range");
    _range["sensor"] = newSchema.ranges[index].sensor;
    _range["upper"] = newSchema.ranges[index].upper;
    _range["lower"] = newSchema.ranges[index].lower;
    _range["greater"] = newSchema.ranges[index].greater;

    JsonObject timing = _range.createNestedObject("timing");
    timing["enable"] = newSchema.ranges[index].timing.enable;
    timing["working"] = newSchema.ranges[index].timing.workingTimeInSecond;
    timing["waiting"] = newSchema.ranges[index].timing.waitingTimeInSecond;

    return data;
  };
};

// @query
class query_range : public Query {
public:
  explicit query_range() : Query("range", new permission_range()) {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    int index = reqData["index"];
    RangeSchema schema = context->range->model->get();

    DynamicJsonDocument data(256);
    data["index"] = index;
    JsonObject range = data.createNestedObject("range");
    range["sensor"] = schema.ranges[index].sensor;
    range["upper"] = schema.ranges[index].upper;
    range["lower"] = schema.ranges[index].lower;
    range["greater"] = schema.ranges[index].greater;

    JsonObject timing = range.createNestedObject("timing");
    timing["enable"] = schema.ranges[index].timing.enable;
    timing["working"] = schema.ranges[index].timing.workingTimeInSecond;
    timing["waiting"] = schema.ranges[index].timing.waitingTimeInSecond;

    return data;
  };
};

#endif //SG_MCU_RESOLVERS_H
