#include "validationError.h"
#include "combineContext.h"
#include "domain/resolvers.h"
#include "util/util.h"

#include "./permission.h"

#ifndef SG_MCU_PRECONDITION_CRITERIA_RESOLVERS_H
#define SG_MCU_PRECONDITION_CRITERIA_RESOLVERS_H

// @mutation
class mutation_precondition_criteria_save : public Resolvers {
public:
  explicit mutation_precondition_criteria_save(CombineContext *context) :
      Resolvers("precondition_criteria_save", context, new permission_precondition_criteria_save(context)) {};

  JsonDocument resolve(JsonObject reqData) override {
    int index = reqData["index"];

    PrecCriteriaSchema criteriaSchema = context->precondition->criteriaModel->get();
    criteriaSchema.criterias[index].sensor = reqData["criteria"]["sensor"];
    criteriaSchema.criterias[index].criteria = reqData["criteria"]["criteria"];
    criteriaSchema.criterias[index].greater = reqData["criteria"]["greater"];

    int writeOps = context->precondition->criteriaModel->save(criteriaSchema);

    PrecCriteriaSchema newCriteriaSchema = context->precondition->criteriaModel->get();
    DynamicJsonDocument data(256);
    data["index"] = index;
    data["writeOps"] = writeOps;
    data["sensor"] = newCriteriaSchema.criterias[index].sensor;
    data["criteria"] = newCriteriaSchema.criterias[index].criteria;
    data["greater"] = newCriteriaSchema.criterias[index].greater;

    return data;
  };
};

class query_precondition_criteria : public Resolvers {
public:
  explicit query_precondition_criteria(CombineContext *context) :
      Resolvers("precondition_criteria_save", context, new permission_precondition_criteria(context)) {};

  JsonDocument resolve(JsonObject reqData) override {
    int index = reqData["index"];
    PrecCriteriaSchema criteriaSchema = context->precondition->criteriaModel->get();

    DynamicJsonDocument data(256);
    data["index"] = index;
    data["sensor"] = criteriaSchema.criterias[index].sensor;
    data["criteria"] = criteriaSchema.criterias[index].criteria;
    data["greater"] = criteriaSchema.criterias[index].greater;

    return data;
  };
};

#endif //SG_MCU_RESOLVERS_H
