#include "domain/query.h"
#include "domain/mutation.h"
#include "validationError.h"
#include "combineContext.h"
#include "util/util.h"

#include "./permission.h"

#ifndef SG_MCU_DEBUG_RESOLVERS_H
#define SG_MCU_DEBUG_RESOLVERS_H

// @query
class query_configuration : public Query {
public:
  explicit query_configuration() : Query("configuration") {};

  JsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    DynamicJsonDocument data(512);
    data["debug"] = Debug::isDebuggingMode();

    return data;
  }
};

class mutation_debug_save : public Mutation {
public:
  explicit mutation_debug_save() : Mutation("debug_save", new permission_debug_save) {};

  JsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    ConfigurationSchema schema = context->config->model->get();
    schema.debug = reqData["debug"];

    int writeOps = context->config->model->save(schema);
    Debug::update();

    DynamicJsonDocument data(512);
    data["writeOps"] = writeOps;
    data["debug"] = Debug::isDebuggingMode();
    return data;
  }
};
#endif //SG_MCU_DEBUG_RESOLVERS_H
