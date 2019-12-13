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
    ControlSchema controlSchema = context->control->model->get();
    controlSchema.type = ControlStringToEnum(reqData["type"]);

    ChannelSchema channelSchema = context->channel->model->get();
    for (auto & channel : channelSchema.channels) {
       channel.isActive = false;
    }
    context->channel->model->save(channelSchema);

    SequenceSchema sequenceSchema = context->sequence->model->get();
    sequenceSchema.sequence.isActive = false;
    context->sequence->model->save(sequenceSchema);

    DynamicJsonDocument data(64);
    if (context->control->core->getType() == controlSchema.type) {
      // NO OP
      data["type"] = reqData["type"];
      data["op"] = "noop";

      return data;
    }

    context->control->core->setTypeAndDeactivateControls(controlSchema.type);
    int writeOps = context->control->model->save(controlSchema);

    data["type"] = ControlEnumToString(context->control->core->getType());
    data["op"] = "updated";
    data["writeOps"] = writeOps;
    return data;
  }
};

#endif //SG_MCU_CONTROL_RESOLVERS_H
