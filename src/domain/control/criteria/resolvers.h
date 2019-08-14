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

  String resolve(JsonObject reqJson) override {
    if (reqJson["data"].isNull() || reqJson["data"]["index"].isNull()) {
      InvalidInputError error("index is not specified.");
      return error.toJsonString();
    }

    if (reqJson["data"]["index"] < 0 || reqJson["data"]["index"] > 7) {
      InvalidInputError error("index out of range.");
      return error.toJsonString();
    }

    int index = reqJson["data"]["index"];
    CriteriaSchema schema = context->criteriaContext->model->get();

    schema.criterias[index].sensor = reqJson["data"]["criteria"]["sensor"];
    schema.criterias[index].criteria = reqJson["data"]["criteria"]["criteria"];
    schema.criterias[index].greater = reqJson["data"]["criteria"]["greater"];

    int writeOps = context->criteriaContext->model->save(schema);

    CriteriaSchema newSchema = context->criteriaContext->model->get();

    StaticJsonDocument<256> data;
    data["index"] = index;
    data["writeOps"] = writeOps;
    JsonObject criteria = data.createNestedObject("criteria");
    criteria["sensor"] = newSchema.criterias[index].sensor;
    criteria["criteria"] = newSchema.criterias[index].criteria;
    criteria["greater"] = newSchema.criterias[index].greater;

    JsonTopic topic(reqJson["topic"], reqJson["method"], data.as<JsonObject>());
    return topic.toString();
  };
};

// @query: date
class query_criteria : public Resolvers {
public:
  explicit query_criteria(CombineContext *context) : Resolvers("criteria", context) {};

  String resolve(JsonObject reqJson) override {
    if (reqJson["data"].isNull() || reqJson["data"]["index"].isNull()) {
      InvalidInputError error("index is not specified.");
      return error.toJsonString();
    }

    if (reqJson["data"]["index"] < 0 || reqJson["data"]["index"] > 7) {
      InvalidInputError error("index out of range.");
      return error.toJsonString();
    }

    int index = reqJson["data"]["index"];
    CriteriaSchema schema = context->criteriaContext->model->get();

    StaticJsonDocument<256> data;
    data["index"] = index;
    JsonObject criteria = data.createNestedObject("criteria");
    criteria["sensor"] = schema.criterias[index].sensor;
    criteria["criteria"] = schema.criterias[index].criteria;
    criteria["greater"] = schema.criterias[index].greater;

    JsonTopic topic(reqJson["topic"], reqJson["method"], data.as<JsonObject>());
    return topic.toString();
  };
};

#endif //SG_MCU_RESOLVERS_H
