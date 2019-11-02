#include "validationError.h"
#include "combineContext.h"
#include "domain/resolvers.h"
#include "util/util.h"
#include "./permission.h"
#include "./util/resolveControlEnum.h"
#ifndef SG_MCU_CONTROL_RESOLVERS_H
#define SG_MCU_CONTROL_RESOLVERS_H

// @query
class query_control_type : public Resolvers {
public:
  explicit query_control_type(CombineContext *context) :
      Resolvers("control_type", context) {};

  JsonDocument resolve(JsonObject reqData) override {
    DynamicJsonDocument data(64);
    data["type"] = ControlEnumToString(context->control->core->getType());
    return data;
  }
};

class mutation_control_type_save : public Resolvers {
public:
  explicit mutation_control_type_save(CombineContext *context) :
      Resolvers("control_type_save", context, new permission_control_type_save(context)) {};

  JsonDocument resolve(JsonObject reqData) override {
    ControlSchema schema = context->control->model->get();
    schema.type = ControlStringToEnum(reqData["type"]);

    DynamicJsonDocument data(64);
    bool isActive = context->control->core->checkAndActiveControl(schema.type);
    if (!isActive) {
      // NO OP
      data["type"] = reqData["type"];
      data["op"] = "noop";

      return data;
    }

    int writeOps = context->control->model->save(schema);

    data["type"] = ControlEnumToString(context->control->core->getType());
    data["op"] = "updated";
    data["writeOps"] = writeOps;
    return data;
  }
};

#endif //SG_MCU_CONTROL_RESOLVERS_H
