#include <ArduinoJson.h>

#include <utility>
#include "combineContext.h"
#include "domain/permission.h"

#ifndef SG_MCU_RESOLVERS_H
#define SG_MCU_RESOLVERS_H

class Resolvers {
public:
  Resolvers(std::string , CombineContext *, Permission *);
  std::string getName() {return name;}
  virtual JsonDocument resolve(JsonObject reqData) {
    DynamicJsonDocument json(64);
    return json;
  }

  JsonDocument exec(JsonObject reqData) {
    if (permission) {
      permission->resolve(reqData);
    }

    return resolve(reqData);
  }

protected:
  std::string name;
  CombineContext *context;
  Permission *permission;
};

Resolvers::Resolvers(std::string name, CombineContext *context, Permission *permission = nullptr) :
  name(std::move(name)),
  context(context),
  permission(permission){};

#endif //SG_MCU_RESOLVERS_H
