#include "util/util.h"

#include "domain/query.h"
#include "validationError.h"
#include "combineContext.h"

#ifndef SG_MCU_INFO_RESOLVERS_H
#define SG_MCU_INFO_RESOLVERS_H

// @query: info
class query_info : public Query {
public:
  explicit query_info() : Query("info") {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    DynamicJsonDocument data(128);
    data["version"] = VERSION; //TODO: fix this
    data["model"] = SG_MODEL;
#ifdef SG_MODE_DEVELOPMENT
    data["device_env"] = "development";
#else
    data["device_env"] = "prod";
#endif
    return data;
  };
};

class query_device_status : public Query {
public:
  explicit query_device_status() : Query("device_status") {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    DynamicJsonDocument data(128);
    data["heap"] = xPortGetFreeHeapSize();
    data["bc"] = bootCount;
    return data;
  };
};

#endif //SG_MCU_RESOLVERS_H
