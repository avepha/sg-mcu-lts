#include "util/util.h"


#include "validationError.h"
#include "combineContext.h"

#include "./permission.h"

#ifndef SG_MCU_CRITERIA_RESOLVERS_H
#define SG_MCU_CRITERIA_RESOLVERS_H

// @mutation
class mutation_criteria_save : public Mutation {
public:
  explicit mutation_criteria_save() : Mutation("criteria_save", new permission_criteria_save()) {};

  JsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    int index = reqData["index"];
    CriteriaSchema schema = context->criteria->model->get();

    JsonObject criteria = reqData["criteria"];

    schema.criterias[index].sensor = criteria["sensor"];
    schema.criterias[index].criteria = criteria["criteria"];
    schema.criterias[index].greater = criteria["greater"];

    if (!criteria["timing"].isNull()) {
      schema.criterias[index].timing.enable = true;
      schema.criterias[index].timing.workingTimeInSecond = criteria["timing"]["working"];
      schema.criterias[index].timing.waitingTimeInSecond = criteria["timing"]["waiting"];
    }
    else {
      schema.criterias[index].timing.enable = false;
    }

    int writeOps = context->criteria->model->save(schema);

    CriteriaSchema newSchema = context->criteria->model->get();

    DynamicJsonDocument data(256);
    data["index"] = index;
    data["writeOps"] = writeOps;
    data["sensor"] = newSchema.criterias[index].sensor;
    data["criteria"] = newSchema.criterias[index].criteria;
    data["greater"] = newSchema.criterias[index].greater;

    JsonObject timing = data.createNestedObject("timing");
    timing["enable"] = newSchema.criterias[index].timing.enable;
    timing["working"] = newSchema.criterias[index].timing.workingTimeInSecond;
    timing["waiting"] = newSchema.criterias[index].timing.waitingTimeInSecond;

    return data;
  };
};

// @query: date
class query_criteria : public Query {
public:
  explicit query_criteria() : Query("criteria", new permission_criteria()) {};

  JsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    int index = reqData["index"];
    CriteriaSchema schema = context->criteria->model->get();

    DynamicJsonDocument data(256);
    data["index"] = index;
    JsonObject criteria = data.createNestedObject("criteria");
    criteria["sensor"] = schema.criterias[index].sensor;
    criteria["criteria"] = schema.criterias[index].criteria;
    criteria["greater"] = schema.criterias[index].greater;

    JsonObject timing = criteria.createNestedObject("timing");
    timing["enable"] = schema.criterias[index].timing.enable;
    timing["working"] = schema.criterias[index].timing.workingTimeInSecond;
    timing["waiting"] = schema.criterias[index].timing.waitingTimeInSecond;

    return data;
  };
};

#endif //SG_MCU_RESOLVERS_H
