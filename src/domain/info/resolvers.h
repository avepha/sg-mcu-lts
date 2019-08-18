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

  String resolve(JsonObject reqJson) override {
    StaticJsonDocument<128> data;
    data["version"] = VERSION;
    JsonTopic topic(reqJson["topic"], reqJson["method"], data.as<JsonObject>());
    return topic.toString();
  };
};

#endif //SG_MCU_RESOLVERS_H
