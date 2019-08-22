#include "util/util.h"

#include "../resolvers.h"
#include "validationError.h"
#include "combineContext.h"

#ifndef SG_MCU_INFO_RESOLVERS_H
#define SG_MCU_INFO_RESOLVERS_H

// @query: info
class query_info : public Resolvers {
public:
  explicit query_info(CombineContext *context) : Resolvers("info", context) {};

  JsonDocument resolve(JsonObject reqJson) override {
    DynamicJsonDocument data(128);
    data["version"] = VERSION;
    return data;
  };
};

#endif //SG_MCU_RESOLVERS_H
