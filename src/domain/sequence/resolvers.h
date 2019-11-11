#include "validationError.h"
#include "combineContext.h"

#include "util/util.h"
#include "./permission.h"
#include "domain/sequence-control/util/resolveSequenceControlEnum.h"
#include "domain/precondition/util/resolvePreconditionEnum.h"

#ifndef SG_MCU_SEQUENCE_RESOLVERS_H
#define SG_MCU_SEQUENCE_RESOLVERS_H

// @query
class query_sequence : public Query {
public:
  explicit query_sequence() : Query("sequence") {};

  JsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    SequenceSchema schema = context->sequence->model->get();
    SequenceSchema::Sequence sequence = schema.sequence;

    DynamicJsonDocument data(1024);
    data["isActive"] = sequence.isActive;
    JsonObject control = data.createNestedObject("control");
    control["type"] = SequenceControlEnumToString(sequence.control.type);
    JsonArray channelOrders = control.createNestedArray("channelOrderAndTiming");
    for (int i = 0; i < sequence.control.channelOrderAndTimingSize; i++) {
      JsonObject jo = channelOrders.createNestedObject();
      jo["channel"] = sequence.control.channelOrderAndTiming[i].channel;
      jo["workingTimeInSec"] = sequence.control.channelOrderAndTiming[i].workingTimeInSec;
    }

    JsonArray jarrPreconditions = data.createNestedArray("preconditions");
    for (int i = 0; i < sizeof(sequence.preconditions) / sizeof(sequence.preconditions[0]); i++) {
      SequenceSchema::Sequence::Precondition precondition = sequence.preconditions[i];
      if (precondition.type != PREC_NONE) {
        JsonObject joPrecondition = jarrPreconditions.createNestedObject();
        joPrecondition["type"] = PreconditionEnumToString(precondition.type);
        joPrecondition["value"] = precondition.value;
      }
    }

    return data;
  }
};

class query_sequence_state : public Query {
public:
  explicit query_sequence_state() : Query("sequence_state", new permission_sequence_state()) {};

  JsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    SequenceControl *sequenceControl = context->sequence->core->getControl();
    SEQUENCE_CONTROL_TYPE_ENUM type = sequenceControl->getType();
    DynamicJsonDocument data(1024);
    JsonArray chainOfControlAndPreconditions = data.createNestedArray("states");
    switch (type) {
      case SEQ_CTRL_TIMER: {
        auto *ctrlCore = (SequenceTimerCore *) (sequenceControl);
        chainOfControlAndPreconditions.add(ctrlCore->getControlState().report());
        break;
      }
      case SEQ_CTRL_CRITERIA: {
        auto *ctrlCore = (SequenceCriteriaCore *) (sequenceControl);
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
      case SEQ_CTRL_RANGE: {
        auto *ctrlCore = (SequenceRangeCore *) (sequenceControl);
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
      default: {
        break;
      }
    }
    return data;
  }
};

class mutation_sequence_activate : public Mutation {
public:
  explicit mutation_sequence_activate() : Mutation("sequence_activate",new permission_sequence_activate) {};

  JsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    SequenceSchema schema = context->sequence->model->get();
    schema.sequence.isActive = reqData["isActive"];

    int writeOps = context->sequence->model->save(schema);

    SequenceSchema newSchema = context->sequence->model->get();

    DynamicJsonDocument data(1024);
    data["writeOps"] = writeOps;
    data["isActive"] = newSchema.sequence.isActive;

    context->sequence->core->activateControls();
    return data;
  }
};

class mutation_sequence_save : public Mutation {
public:
  explicit mutation_sequence_save() : Mutation("sequence_save", new permission_sequence_save()) {};

  JsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    SequenceSchema schema = context->sequence->model->get();

    schema.sequence.control.type = SequenceControlStringToEnum(reqData["control"]["type"]);
    for (int i = 0; i < reqData["control"]["channelOrderAndTiming"].size(); i++) {
      schema.sequence.control.channelOrderAndTiming[i].channel = reqData["control"]["channelOrderAndTiming"][i]["channel"];
      schema.sequence.control.channelOrderAndTiming[i].workingTimeInSec = reqData["control"]["channelOrderAndTiming"][i]["workingTimeInSec"];
    }
    schema.sequence.control.channelOrderAndTimingSize = reqData["control"]["channelOrderAndTiming"].size();

    // set precondition by providing args
    for (int i = 0; i < reqData["preconditions"].size(); i++) {
      schema.sequence.preconditions[i].type = PreconditionStringToEnum(reqData["preconditions"][i]["type"]);
      schema.sequence.preconditions[i].value = reqData["preconditions"][i]["value"];
    }

    // set precondition to none if providing args size < 3
    for (int i = reqData["preconditions"].size(); i < 3; i++) {
      schema.sequence.preconditions[i].type = PREC_NONE;
      schema.sequence.preconditions[i].value = 0;
    }

    schema.sequence.isActive = false;
    context->sequence->core->deactivateControls();
    int writeOps = context->sequence->model->save(schema);
    delay(10);

    SequenceSchema newSchema = context->sequence->model->get();
    SequenceSchema::Sequence sequence = newSchema.sequence;

    DynamicJsonDocument data(1024);
    data["isActive"] = sequence.isActive;
    data["writeOps"] = writeOps;
    JsonObject control = data.createNestedObject("control");
    control["type"] = SequenceControlEnumToString(sequence.control.type);
    JsonArray channelOrderAndTiming = control.createNestedArray("channelOrderAndTiming");
    for (int i = 0; i < sequence.control.channelOrderAndTimingSize; i++) {
      JsonObject jo = channelOrderAndTiming.createNestedObject();
      jo["channel"] = sequence.control.channelOrderAndTiming[i].channel;
      jo["workingTimeInSec"] = sequence.control.channelOrderAndTiming[i].workingTimeInSec;
    }

    JsonArray jarrPreconditions = data.createNestedArray("preconditions");
    for (int i = 0; i < sizeof(sequence.preconditions) / sizeof(sequence.preconditions[0]); i++) {
      SequenceSchema::Sequence::Precondition precondition = sequence.preconditions[i];
      if (precondition.type != PREC_NONE) {
        JsonObject joPrecondition = jarrPreconditions.createNestedObject();
        joPrecondition["type"] = PreconditionEnumToString(precondition.type);
        joPrecondition["value"] = precondition.value;
      }
    }
    return data;
  }
};

#endif //SG_MCU_SEQUENCE_RESOLVERS_H
