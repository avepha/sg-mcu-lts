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

  String resolve(JsonObject reqJson) override {
    if (reqJson["data"].isNull() || reqJson["data"]["index"].isNull() || reqJson["data"]["timers"].isNull()) {
      InvalidInputError err("index or timers field is not specified.");
      return err.toJsonString();
    }

    if (reqJson["data"]["index"].as<int>() < 0 || reqJson["data"]["index"].as<int>() > 7) {
      InvalidInputError err("index out of range.");
      return err.toJsonString();
    }

    int index = reqJson["data"]["index"].as<int>();

    if (!reqJson["data"]["timers"].isNull()) {
      PreConditionTimerSchema timerSchema = context->preConditionContext->timerModel->get();
      int size = reqJson["data"]["timers"].size();

      timerSchema.timers[index].size = size;
      for(int i = 0 ; i < size; i++) {
        timerSchema.timers[index].data[i][0] = reqJson["data"]["timers"][i][0];
        timerSchema.timers[index].data[i][1] = reqJson["data"]["timers"][i][1];
      }

      int writeOps = context->preConditionContext->timerModel->save(timerSchema);
      delay(10);

      PreConditionTimerSchema newSchema = context->preConditionContext->timerModel->get();
      StaticJsonDocument<1024> data;
      data["index"] = index;
      data["writeOps"] = writeOps;
      JsonArray timers = data.createNestedArray("timers");
      for (int i = 0; i < newSchema.timers[index].size; i++) {
        JsonArray d_i = timers.createNestedArray();
        d_i.add(newSchema.timers[index].data[i][0]);
        d_i.add(newSchema.timers[index].data[i][1]);
      }

      JsonTopic response(reqJson["topic"], reqJson["method"], data.as<JsonObject>());
      return response.toString();
    }

    if(!reqJson["data"]["criteria"].isNull()) {
      PreConditionCriteriaSchema criteriaSchema = context->preConditionContext->criteriaModel->get();
      criteriaSchema.criterias[index].sensor = reqJson["data"]["criteria"]["sensor"];
      criteriaSchema.criterias[index].criteria = reqJson["data"]["criteria"]["criteria"];
      criteriaSchema.criterias[index].greater = reqJson["data"]["criteria"]["greater"];

      int writeOps = context->preConditionContext->criteriaModel->save(criteriaSchema);

      PreConditionCriteriaSchema newCriteriaSchema = context->preConditionContext->criteriaModel->get();
      StaticJsonDocument<1024> data;
      data["index"] = index;
      data["writeOps"] = writeOps;
      JsonObject criteria = data.createNestedObject("criteria");
      criteria["sensor"] = newCriteriaSchema.criterias[index].sensor;
      criteria["criteria"] = newCriteriaSchema.criterias[index].criteria;
      criteria["greater"] = newCriteriaSchema.criterias[index].greater;

      JsonTopic response(reqJson["topic"], reqJson["method"], data.as<JsonObject>());
      return response.toString();
    }

    InvalidInputError err;
    return err.toJsonString();
  };
};

// @query:
class query_precondition : public Resolvers {
public:
  explicit query_precondition(CombineContext *context) : Resolvers("precondition", context) {};

  String resolve(JsonObject reqJson) override {
    if (reqJson["data"]["type"].isNull() || reqJson["data"]["index"].isNull()) {
      InvalidInputError err("type Or index field is not specified.");
      return err.toJsonString();
    }

    if (reqJson["data"]["index"].as<int>() < 0 || reqJson["data"]["index"].as<int>() > 7) {
      InvalidInputError err("index out of range.");
      return err.toJsonString();
    }

    int index = reqJson["data"]["index"];
    String type = reqJson["data"]["type"];

    if (type == "timer") {
      PreConditionTimerSchema timerSchema = context->preConditionContext->timerModel->get();
      StaticJsonDocument<1024> data;
      data["index"] = index;
      JsonArray timers = data.createNestedArray("timers");
      for (int i = 0; i < timerSchema.timers[index].size; i++) {
        JsonArray d_i = timers.createNestedArray();
        d_i.add(timerSchema.timers[index].data[i][0]);
        d_i.add(timerSchema.timers[index].data[i][1]);
      }

      JsonTopic response(reqJson["topic"], reqJson["method"], data.as<JsonObject>());
      return response.toString();
    }

    if (type == "criteria") {
      PreConditionCriteriaSchema criteriaSchema = context->preConditionContext->criteriaModel->get();
      StaticJsonDocument<1024> data;
      data["index"] = index;
      JsonObject criteria = data.createNestedObject("criteria");
      criteria["sensor"] = criteriaSchema.criterias[index].sensor;
      criteria["criteria"] = criteriaSchema.criterias[index].criteria;
      criteria["greater"] = criteriaSchema.criterias[index].greater;

      JsonTopic response(reqJson["topic"], reqJson["method"], data.as<JsonObject>());
      return response.toString();
    }

    InvalidInputError err("No such the provided type.");
    return err.toJsonString();
  };
};


#endif //SG_MCU_RESOLVERS_H
