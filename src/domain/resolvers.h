#include <ArduinoJson.h>

#include "combineContext.h"
#include "domain/permission.h"

#ifndef SG_MCU_RESOLVERS_H
#define SG_MCU_RESOLVERS_H

class Resolvers {
public:
  Resolvers(std::string, Permission *);

  std::string getName() { return name; }

  virtual JsonDocument resolve(JsonObject reqData, CombineContext *context) {
    DynamicJsonDocument json(64);
    return json;
  }

  JsonDocument exec(JsonObject reqData, CombineContext *context) {
    if (permission) {
      permission->resolve(reqData, context);
    }

    return resolve(reqData, context);
  }

protected:
  std::string name;
  Permission *permission;
};

Resolvers::Resolvers(std::string name, Permission *permission = nullptr) :
    name(std::move(name)),
    permission(permission) {};
#endif //SG_MCU_RESOLVERS_H
