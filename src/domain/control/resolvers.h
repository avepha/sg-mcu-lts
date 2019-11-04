#include "validationError.h"
#include "combineContext.h"

#include "util/util.h"
#include "./permission.h"
#include "./util/resolveControlEnum.h"
#ifndef SG_MCU_CONTROL_RESOLVERS_H
#define SG_MCU_CONTROL_RESOLVERS_H

// @query
class query_control_type : public Query {
public:
  explicit query_control_type() : Query("control_type") {};

  JsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    DynamicJsonDocument data(64);
    data["type"] = ControlEnumToString(context->control->core->getType());
    return data;
  }
};

class mutation_control_type_save : public Mutation {
public:
  explicit mutation_control_type_save() : Mutation("control_type_save", new permission_control_type_save()) {};

  JsonDocument resolve(JsonObject reqData, CombineContext *context) override {
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
