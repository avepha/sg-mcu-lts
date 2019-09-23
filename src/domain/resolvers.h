#include <ArduinoJson.h>
#include "combineContext.h"
#include "domain/permission.h"

#ifndef SG_MCU_RESOLVERS_H
#define SG_MCU_RESOLVERS_H

class Resolvers {
public:
  Resolvers(String , CombineContext *, Permission *);
  String getName() {return name;}
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
  String name;
  CombineContext *context;
  Permission *permission;
};

Resolvers::Resolvers(String name, CombineContext *context, Permission *permission = nullptr) :
  name(name),
  context(context),
  permission(permission){};

#endif //SG_MCU_RESOLVERS_H
