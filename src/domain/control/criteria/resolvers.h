#include "util/util.h"

#include "domain/resolvers.h"
#include "validationError.h"
#include "combineContext.h"

#include "./permission.h"

#ifndef SG_MCU_CRITERIA_RESOLVERS_H
#define SG_MCU_CRITERIA_RESOLVERS_H

// @mutation
class mutation_criteria_save : public Resolvers {
public:
  explicit mutation_criteria_save(CombineContext *context) : Resolvers("criteria_save", context, new permission_criteria_save(context)) {};

  JsonDocument resolve(JsonObject reqData) override {
    int index = reqData["index"];
    CriteriaSchema schema = context->criteriaContext->model->get();

    schema.criterias[index].sensor = reqData["sensor"];
    schema.criterias[index].criteria = reqData["criteria"];
    schema.criterias[index].greater = reqData["greater"];

    if (!reqData["timing"].isNull()) {
      schema.criterias[index].timing.enable = true;
      schema.criterias[index].timing.workingTimeInSecond = reqData["timing"]["working"];
      schema.criterias[index].timing.waitingTimeInSecond = reqData["timing"]["waiting"];
    }
    else {
      schema.criterias[index].timing.enable = false;
    }

    int writeOps = context->criteriaContext->model->save(schema);

    CriteriaSchema newSchema = context->criteriaContext->model->get();

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
class query_criteria : public Resolvers {
public:
  explicit query_criteria(CombineContext *context) : Resolvers("criteria", context, new permission_criteria(context)) {};

  JsonDocument resolve(JsonObject reqData) override {
    int index = reqData["index"];
    CriteriaSchema schema = context->criteriaContext->model->get();

    DynamicJsonDocument data(256);
    data["index"] = index;
    data["sensor"] = schema.criterias[index].sensor;
    data["criteria"] = schema.criterias[index].criteria;
    data["greater"] = schema.criterias[index].greater;

    JsonObject timing = data.createNestedObject("timing");
    timing["enable"] = schema.criterias[index].timing.enable;
    timing["working"] = schema.criterias[index].timing.workingTimeInSecond;
    timing["waiting"] = schema.criterias[index].timing.waitingTimeInSecond;

    return data;
  };
};

#endif //SG_MCU_RESOLVERS_H
