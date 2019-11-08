#include "validationError.h"
#include "combineContext.h"

#include "util/util.h"
#include "./permission.h"
#include "domain/continuous-control/util/resolveContinuousControlEnum.h"
#include "domain/precondition/util/resolvePreconditionEnum.h"

#ifndef SG_MCU_CONTINUOUS_RESOLVERS_H
#define SG_MCU_CONTINUOUS_RESOLVERS_H

// @query
class query_continuous : public Query {
public:
  explicit query_continuous() : Query("continuous") {};

  JsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    ContinuousSchema schema = context->continuous->model->get();
    ContinuousSchema::Continuous continuous = schema.continuous;

    DynamicJsonDocument data(1024);
    data["isActive"] = continuous.isActive;
    JsonObject control = data.createNestedObject("control");
    control["type"] = ContinuousControlEnumToString(continuous.control.type);
    JsonArray channelOrders = control.createNestedArray("channelOrderAndTiming");
    for (int i = 0; i < continuous.control.channelOrderAndTimingSize; i++) {
      JsonObject jo = channelOrders.createNestedObject();
      jo["channel"] = continuous.control.channelOrderAndTiming[i].channel;
      jo["workingTimeInSec"] = continuous.control.channelOrderAndTiming[i].workingTimeInSec;
    }

    JsonArray jarrPreconditions = data.createNestedArray("preconditions");
    for (int i = 0; i < sizeof(continuous.preconditions) / sizeof(continuous.preconditions[0]); i++) {
      ContinuousSchema::Continuous::Precondition precondition = continuous.preconditions[i];
      if (precondition.type != PREC_NONE) {
        JsonObject joPrecondition = jarrPreconditions.createNestedObject();
        joPrecondition["type"] = PreconditionEnumToString(precondition.type);
        joPrecondition["value"] = precondition.value;
      }
    }

    return data;
  }
};

//TODO: complete this
class query_continuous_state : public Query {
public:
  explicit query_continuous_state() : Query("continuous_state", new permission_continuous_state()) {};

  JsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    ContinuousControl *continuousControl = context->continuous->core->getControl();
    CONTINUOUS_CONTROL_TYPE_ENUM type = continuousControl->getType();
    DynamicJsonDocument data(1024);
    JsonArray chainOfControlAndPreconditions = data.createNestedArray("states");
    switch (type) {
      case CON_CTRL_TIMER: {
        auto *ctrlCore = (ContinuousTimerCore *) (continuousControl);
        chainOfControlAndPreconditions.add(ctrlCore->getControlState().report());
        break;
      }
      case CON_CTRL_CRITERIA: {
        auto *ctrlCore = (ContinuousCriteriaCore *) (continuousControl);
        for (int i = 0; i < ctrlCore->getPreconditionSize(); i++) {
          if (ctrlCore->getPreconditionAt(i)->getType() == PREC_CRITERIA) {
            auto *precCoreAtN = (PrecCriteriaCore *) (ctrlCore->getPreconditionAt(i));
            chainOfControlAndPreconditions.add(precCoreAtN->getPreconditionState().report());
          }
          else if (ctrlCore->getPreconditionAt(i)->getType() == PREC_TIMER) {
            auto *precCoreAtN = (PrecTimerCore *) (ctrlCore->getPreconditionAt(i));
            chainOfControlAndPreconditions.add(precCoreAtN->getPreconditionState().report());
          }
          else if (ctrlCore->getPreconditionAt(i)->getType() == PREC_RANGE) {
            auto *precCoreAtN = (PrecRangeCore *) (ctrlCore->getPreconditionAt(i));
            chainOfControlAndPreconditions.add(precCoreAtN->getPreconditionState().report());
          }
        }
        chainOfControlAndPreconditions.add(ctrlCore->getControlState().report());
        break;
      }
      case CON_CTRL_RANGE: {
        break;
      }
      default: {
        break;
      }
    }
    return data;
  }
};

class mutation_continuous_activate : public Mutation {
public:
  explicit mutation_continuous_activate() : Mutation("continuous_activate",new permission_continuous_activate) {};

  JsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    ContinuousSchema schema = context->continuous->model->get();
    schema.continuous.isActive = reqData["isActive"];

    int writeOps = context->continuous->model->save(schema);

    ContinuousSchema newSchema = context->continuous->model->get();

    DynamicJsonDocument data(1024);
    data["writeOps"] = writeOps;
    data["isActive"] = newSchema.continuous.isActive;

    context->continuous->core->activateControls();
    return data;
  }
};

class mutation_continuous_save : public Mutation {
public:
  explicit mutation_continuous_save() : Mutation("continuous_save", new permission_continuous_save()) {};

  JsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    ContinuousSchema schema = context->continuous->model->get();

    schema.continuous.control.type = ContinuousControlStringToEnum(reqData["control"]["type"]);
    for (int i = 0; i < reqData["control"]["channelOrderAndTiming"].size(); i++) {
      schema.continuous.control.channelOrderAndTiming[i].channel = reqData["control"]["channelOrderAndTiming"][i]["channel"];
      schema.continuous.control.channelOrderAndTiming[i].workingTimeInSec = reqData["control"]["channelOrderAndTiming"][i]["workingTimeInSec"];
    }
    schema.continuous.control.channelOrderAndTimingSize = reqData["control"]["channelOrderAndTiming"].size();

    // set precondition by providing args
    for (int i = 0; i < reqData["preconditions"].size(); i++) {
      schema.continuous.preconditions[i].type = PreconditionStringToEnum(reqData["preconditions"][i]["type"]);
      schema.continuous.preconditions[i].value = reqData["preconditions"][i]["value"];
    }

    // set precondition to none if providing args size < 3
    for (int i = reqData["preconditions"].size(); i < 3; i++) {
      schema.continuous.preconditions[i].type = PREC_NONE;
      schema.continuous.preconditions[i].value = 0;
    }

    schema.continuous.isActive = false;
    context->continuous->core->deactivateControls();
    context->continuous->model->save(schema);
    delay(10);

    ContinuousSchema newSchema = context->continuous->model->get();
    ContinuousSchema::Continuous continuous = newSchema.continuous;

    DynamicJsonDocument data(1024);
    data["isActive"] = continuous.isActive;
    JsonObject control = data.createNestedObject("control");
    control["type"] = ContinuousControlEnumToString(continuous.control.type);
    JsonArray channelOrderAndTiming = control.createNestedArray("channelOrderAndTiming");
    for (int i = 0; i < continuous.control.channelOrderAndTimingSize; i++) {
      JsonObject jo = channelOrderAndTiming.createNestedObject();
      jo["channel"] = continuous.control.channelOrderAndTiming[i].channel;
      jo["workingTimeInSec"] = continuous.control.channelOrderAndTiming[i].workingTimeInSec;
    }

    JsonArray jarrPreconditions = data.createNestedArray("preconditions");
    for (int i = 0; i < sizeof(continuous.preconditions) / sizeof(continuous.preconditions[0]); i++) {
      ContinuousSchema::Continuous::Precondition precondition = continuous.preconditions[i];
      if (precondition.type != PREC_NONE) {
        JsonObject joPrecondition = jarrPreconditions.createNestedObject();
        joPrecondition["type"] = PreconditionEnumToString(precondition.type);
        joPrecondition["value"] = precondition.value;
      }
    }
    return data;
  }
};

#endif //SG_MCU_CONTINUOUS_RESOLVERS_H
