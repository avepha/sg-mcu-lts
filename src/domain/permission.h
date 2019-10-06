#include<ArduinoJson.h>
#include "combineContext.h"

#ifndef SG_MCU_PERMISSION_H
#define SG_MCU_PERMISSION_H

class Permission {
public:
  Permission(CombineContext *context) : context(context) {};
  virtual void resolve(JsonObject) {};
private:
  CombineContext *context;
};


#endif //SG_MCU_PERMISSION_H
