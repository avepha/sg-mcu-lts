#include <ArduinoJson.h>
#include "combineContext.h"

#ifndef SG_MCU_PERMISSION_H
#define SG_MCU_PERMISSION_H

class Permission {
public:
  Permission()= default;;
  virtual void resolve(JsonObject, CombineContext *context) {};
private:
};


#endif //SG_MCU_PERMISSION_H
