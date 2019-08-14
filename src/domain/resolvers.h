#include <ArduinoJson.h>
#include "combineContext.h"
#ifndef SG_MCU_RESOLVERS_H
#define SG_MCU_RESOLVERS_H

class Resolvers {
public:
  //TODO: what is appropriate date type should we return?
  Resolvers(String , CombineContext *);
  String getName() {return name;}
  virtual String resolve() {return "NULL";}
  virtual String resolve(JsonObject reqJson) {return "NULL";}

protected:
  String name;
  CombineContext *context;
};

Resolvers::Resolvers(String name, CombineContext *context) :
  name(name),
  context(context) {};

#endif //SG_MCU_RESOLVERS_H
