#include <ArduinoJson.h>
#include "combineContext.h"
#ifndef SG_MCU_RESOLVERS_H
#define SG_MCU_RESOLVERS_H

class Resolvers {
public:
  Resolvers(String , CombineContext *);
  String getName() {return name;}
  virtual String resolve() { return "NULL";}
  virtual JsonDocument resolve(JsonObject reqJson) {
    DynamicJsonDocument json(64);
    return json;
  }

protected:
  String name;
  CombineContext *context;
};

Resolvers::Resolvers(String name, CombineContext *context) :
  name(name),
  context(context) {};

#endif //SG_MCU_RESOLVERS_H
