#include "domain/query.h"
#include "domain/mutation.h"
#include "validationError.h"
#include "combineContext.h"
#include "util/util.h"
#include "logger/log.h"

#include "./permission.h"

#ifndef SG_MCU_CONFIG_RESOLVERS_H
#define SG_MCU_CONFIG_RESOLVERS_H

// @query
class query_configuration : public Query {
public:
  explicit query_configuration() : Query("configuration") {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    DynamicJsonDocument data(512);
    ConfigurationSchema schema = context->config->model->get();
    data["logLevel"] = LoggerLevelEnumToString(schema.logLevel);
    return data;
  }
};

class mutation_log_level_save : public Mutation {
public:
  explicit mutation_log_level_save() : Mutation("log_level_save", new permission_log_level_save) {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    ConfigurationSchema schema = context->config->model->get();
    schema.logLevel = StringToLoggerLevelEnum(reqData["level"]);

    int writeOps = context->config->model->save(schema);
    delay(10);
    Log::updateLogLevel();

    ConfigurationSchema newSchema = context->config->model->get();
    DynamicJsonDocument data(512);
    data["writeOps"] = writeOps;
    data["logLevel"] = LoggerLevelEnumToString(newSchema.logLevel);
    return data;
  }
};
#endif //SG_MCU_CONFIG_RESOLVERS_H
