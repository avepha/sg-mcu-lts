#include "validationError.h"
#include "combineContext.h"
#include "domain/resolvers.h"
#include "util/util.h"

#ifndef SG_MCU_PRECONDITION_RESOLVERS_H
#define SG_MCU_PRECONDITION_RESOLVERS_H

// @mutation
class mutation_precondition_save : public Resolvers {
public:
  explicit mutation_precondition_save(CombineContext *context) : Resolvers("precondition_save", context) {};

  JsonDocument resolve(JsonObject reqData) override {
    if (reqData.isNull() || reqData["index"].isNull()) {
      InvalidInputError err("index or timers field is not specified.");
      throw err;
    }

    if (reqData["index"] < 0 || reqData["index"] > 7) {
      InvalidInputError err("index out of range.");
      throw err;
    }

    int index = reqData["index"];

    if (!reqData["timers"].isNull()) {
      PreConditionTimerSchema timerSchema = context->preConditionContext->timerModel->get();
      int size = reqData["timers"].size();

      timerSchema.timers[index].size = size;
      for(int i = 0 ; i < size; i++) {
        timerSchema.timers[index].data[i][0] = reqData["timers"][i][0];
        timerSchema.timers[index].data[i][1] = reqData["timers"][i][1];
      }

      int writeOps = context->preConditionContext->timerModel->save(timerSchema);
      delay(10);

      PreConditionTimerSchema newSchema = context->preConditionContext->timerModel->get();
      DynamicJsonDocument data(1024);
      data["index"] = index;
      data["writeOps"] = writeOps;
      JsonArray timers = data.createNestedArray("timers");
      for (int i = 0; i < newSchema.timers[index].size; i++) {
        JsonArray d_i = timers.createNestedArray();
        d_i.add(newSchema.timers[index].data[i][0]);
        d_i.add(newSchema.timers[index].data[i][1]);
      }

      return data;
    }

    if(!reqData["criteria"].isNull()) {
      PreConditionCriteriaSchema criteriaSchema = context->preConditionContext->criteriaModel->get();
      criteriaSchema.criterias[index].sensor = reqData["criteria"]["sensor"];
      criteriaSchema.criterias[index].criteria = reqData["criteria"]["criteria"];
      criteriaSchema.criterias[index].greater = reqData["criteria"]["greater"];

      int writeOps = context->preConditionContext->criteriaModel->save(criteriaSchema);

      PreConditionCriteriaSchema newCriteriaSchema = context->preConditionContext->criteriaModel->get();
      DynamicJsonDocument data(1024);
      data["index"] = index;
      data["writeOps"] = writeOps;
      JsonObject criteria = data.createNestedObject("criteria");
      criteria["sensor"] = newCriteriaSchema.criterias[index].sensor;
      criteria["criteria"] = newCriteriaSchema.criterias[index].criteria;
      criteria["greater"] = newCriteriaSchema.criterias[index].greater;

      return data;
    }

    InvalidInputError err;
    throw err;
  };
};

// @query:
class query_precondition : public Resolvers {
public:
  explicit query_precondition(CombineContext *context) : Resolvers("precondition", context) {};

  JsonDocument resolve(JsonObject reqData) override {
    if (reqData["type"].isNull() || reqData["index"].isNull()) {
      InvalidInputError err("type Or index field is not specified.");
      throw err;
    }

    if (reqData["index"].as<int>() < 0 || reqData["index"].as<int>() > 7) {
      InvalidInputError err("index out of range.");
      throw err;
    }

    int index = reqData["index"];
    String type = reqData["type"];

    if (type == "timer") {
      PreConditionTimerSchema timerSchema = context->preConditionContext->timerModel->get();
      DynamicJsonDocument data(1024);
      data["index"] = index;
      JsonArray timers = data.createNestedArray("timers");
      for (int i = 0; i < timerSchema.timers[index].size; i++) {
        JsonArray d_i = timers.createNestedArray();
        d_i.add(timerSchema.timers[index].data[i][0]);
        d_i.add(timerSchema.timers[index].data[i][1]);
      }

      return data;
    }

    if (type == "criteria") {
      PreConditionCriteriaSchema criteriaSchema = context->preConditionContext->criteriaModel->get();
      DynamicJsonDocument data(1024);
      data["index"] = index;
      JsonObject criteria = data.createNestedObject("criteria");
      criteria["sensor"] = criteriaSchema.criterias[index].sensor;
      criteria["criteria"] = criteriaSchema.criterias[index].criteria;
      criteria["greater"] = criteriaSchema.criterias[index].greater;

      return data;
    }

    InvalidInputError err("No such the provided type.");
    throw err;
  };
};


#endif //SG_MCU_RESOLVERS_H
