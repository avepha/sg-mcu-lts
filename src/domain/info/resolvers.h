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

  JsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    DynamicJsonDocument data(128);
    data["version"] = VERSION; //TODO: fix this
#ifdef SG_TEST
    data["device_env"] = "test";
#endif
    return data;
  };
};

#endif //SG_MCU_RESOLVERS_H
