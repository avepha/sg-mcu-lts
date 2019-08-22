#include "util/util.h"

#include "domain/resolvers.h"
#include "validationError.h"
#include "combineContext.h"

#ifndef SG_MCU_CRITERIA_RESOLVERS_H
#define SG_MCU_CRITERIA_RESOLVERS_H

// @mutation
class mutation_criteria_save : public Resolvers {
public:
  explicit mutation_criteria_save(CombineContext *context) : Resolvers("criteria_save", context) {};

  JsonDocument resolve(JsonObject reqData) override {
    if (reqData.isNull() || reqData["index"].isNull()) {
      InvalidInputError err("index is not specified.");
      throw err;
    }

    if (reqData["index"] < 0 || reqData["index"] > 7) {
      InvalidInputError err("index out of range.");
      throw err;
    }

    int index = reqData["index"];
    CriteriaSchema schema = context->criteriaContext->model->get();

    schema.criterias[index].sensor = reqData["criteria"]["sensor"];
    schema.criterias[index].criteria = reqData["criteria"]["criteria"];
    schema.criterias[index].greater = reqData["criteria"]["greater"];

    int writeOps = context->criteriaContext->model->save(schema);

    CriteriaSchema newSchema = context->criteriaContext->model->get();

    DynamicJsonDocument data(256);
    data["index"] = index;
    data["writeOps"] = writeOps;
    JsonObject criteria = data.createNestedObject("criteria");
    criteria["sensor"] = newSchema.criterias[index].sensor;
    criteria["criteria"] = newSchema.criterias[index].criteria;
    criteria["greater"] = newSchema.criterias[index].greater;

    return data;
  };
};

// @query: date
class query_criteria : public Resolvers {
public:
  explicit query_criteria(CombineContext *context) : Resolvers("criteria", context) {};

  JsonDocument resolve(JsonObject reqData) override {
    if (reqData.isNull() || reqData["index"].isNull()) {
      InvalidInputError err("index is not specified.");
      throw err;
    }

    if (reqData["index"] < 0 || reqData["index"] > 7) {
      InvalidInputError err("index out of range.");
      throw err;
    }

    int index = reqData["index"];
    CriteriaSchema schema = context->criteriaContext->model->get();

    DynamicJsonDocument data(256);
    data["index"] = index;
    JsonObject criteria = data.createNestedObject("criteria");
    criteria["sensor"] = schema.criterias[index].sensor;
    criteria["criteria"] = schema.criterias[index].criteria;
    criteria["greater"] = schema.criterias[index].greater;

    return data;
  };
};

#endif //SG_MCU_RESOLVERS_H
