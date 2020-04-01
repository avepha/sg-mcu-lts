#include "validationError.h"
#include "combineContext.h"

#include "util/util.h"

#include "./permission.h"

#ifndef SG_MCU_PRECONDITION_CRITERIA_RESOLVERS_H
#define SG_MCU_PRECONDITION_CRITERIA_RESOLVERS_H

// @mutation
class mutation_precondition_criteria_save : public Mutation {
public:
  explicit mutation_precondition_criteria_save() : Mutation("precondition_criteria_save", new permission_precondition_criteria_save()) {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    int index = reqData["index"];

    PrecCriteriaSchema criteriaSchema = context->precCriteria->model->get();
    criteriaSchema.criterias[index].sensor = reqData["criteria"]["sensor"];
    criteriaSchema.criterias[index].criteria = reqData["criteria"]["criteria"];
    criteriaSchema.criterias[index].greater = reqData["criteria"]["greater"];

    int writeOps = context->precCriteria->model->save(criteriaSchema);

    PrecCriteriaSchema newCriteriaSchema = context->precCriteria->model->get();
    DynamicJsonDocument data(256);
    data["index"] = index;
    data["writeOps"] = writeOps;
    JsonObject criteria = data.createNestedObject("criteria");
    criteria["sensor"] = newCriteriaSchema.criterias[index].sensor;
    criteria["criteria"] = newCriteriaSchema.criterias[index].criteria;
    criteria["greater"] = newCriteriaSchema.criterias[index].greater;

    return data;
  };
};

class query_precondition_criteria : public Query {
public:
  explicit query_precondition_criteria() : Query("precondition_criteria", new permission_precondition_criteria()) {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    int index = reqData["index"];
    PrecCriteriaSchema criteriaSchema = context->precCriteria->model->get();

    DynamicJsonDocument data(256);
    data["index"] = index;
    JsonObject criteria = data.createNestedObject("criteria");
    criteria["sensor"] = criteriaSchema.criterias[index].sensor;
    criteria["criteria"] = criteriaSchema.criterias[index].criteria;
    criteria["greater"] = criteriaSchema.criterias[index].greater;

    return data;
  };
};

#endif //SG_MCU_RESOLVERS_H
