#include "util/util.h"


#include "validationError.h"
#include "combineContext.h"

#include "./permission.h"

#ifndef SG_MCU_CONTINUOUS_CRITERIA_RESOLVERS_H
#define SG_MCU_CONTINUOUS_CRITERIA_RESOLVERS_H

// @mutation
class mutation_continuous_criteria_save : public Mutation {
public:
  explicit mutation_continuous_criteria_save() : Mutation("continuous_criteria_save", new permission_continuous_criteria_save()) {};

  JsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    ContinuousCriteriaSchema schema = context->continuousCriteria->model->get();

    JsonObject criteria = reqData["criteria"];

    schema.criteria.sensor = criteria["sensor"];
    schema.criteria.criteria = criteria["criteria"];
    schema.criteria.greater = criteria["greater"];

    if (!criteria["timing"].isNull()) {
      schema.criteria.timing.enable = true;
      schema.criteria.timing.waitingTimeInSecond = criteria["timing"]["waiting"];
    }
    else {
      schema.criteria.timing.enable = false;
    }

    int writeOps = context->continuousCriteria->model->save(schema);

    ContinuousCriteriaSchema newSchema = context->continuousCriteria->model->get();

    DynamicJsonDocument data(256);
    data["writeOps"] = writeOps;
    data["sensor"] = newSchema.criteria.sensor;
    data["criteria"] = newSchema.criteria.criteria;
    data["greater"] = newSchema.criteria.greater;

    JsonObject timing = data.createNestedObject("timing");
    timing["enable"] = newSchema.criteria.timing.enable;
    timing["waiting"] = newSchema.criteria.timing.waitingTimeInSecond;

    return data;
  };
};

// @query: date
class query_continuous_criteria : public Query {
public:
  explicit query_continuous_criteria() : Query("continuous_criteria") {};

  JsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    ContinuousCriteriaSchema schema = context->continuousCriteria->model->get();

    DynamicJsonDocument data(256);
    JsonObject criteria = data.createNestedObject("criteria");
    criteria["sensor"] = schema.criteria.sensor;
    criteria["criteria"] = schema.criteria.criteria;
    criteria["greater"] = schema.criteria.greater;

    JsonObject timing = criteria.createNestedObject("timing");
    timing["enable"] = schema.criteria.timing.enable;
    timing["waiting"] = schema.criteria.timing.waitingTimeInSecond;

    return data;
  };
};

#endif //SG_MCU_CONTINUOUS_CRITERIA_RESOLVERS_H
