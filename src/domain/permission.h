#include<ArduinoJson.h>
#include "combineContext.h"

#ifndef SG_MCU_PERMISSION_H
#define SG_MCU_PERMISSION_H

class Permission {
public:
  CombineContext *context;
  Permission(CombineContext *context) : context(context) {};
  virtual void resolve(JsonObject) {};
private:
};


#endif //SG_MCU_PERMISSION_H
