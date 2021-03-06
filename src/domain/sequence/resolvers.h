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

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    SequenceSchema schema = context->sequence->model->get();
    SequenceSchema::Sequence sequence = schema.sequence;

    DynamicJsonDocument data(1024);
    data["isActive"] = sequence.isActive;
    JsonObject control = data.createNestedObject("control");
    control["type"] = SequenceControlEnumToString(sequence.control.type);

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

class query_sequence_order : public Query {
public:
  explicit query_sequence_order() : Query("sequence_order") {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    SequenceSchema schema = context->sequence->model->get();
    DynamicJsonDocument data(1024);
    JsonArray channelOrderAndTiming = data.createNestedArray("channelOrderAndTiming");
    for (int i = 0; i < schema.sequence.channelOrderAndTimingSize; i++) {
      JsonObject jo = channelOrderAndTiming.createNestedObject();
      jo["working"] = schema.sequence.channelOrderAndTiming[i].workingTimeInSec;
      jo["channel"] = schema.sequence.channelOrderAndTiming[i].channel;
    }

    return data;
  }
};

class query_sequence_state : public Query {
public:
  explicit query_sequence_state() : Query("sequence_state", new permission_sequence_state()) {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    SequenceControl *sequenceControl = context->sequence->core->getControl();
    SEQUENCE_CONTROL_TYPE_ENUM type = sequenceControl->getType();
    DynamicJsonDocument data(1024);
    JsonArray chainOfPreconditions = data.createNestedArray("preconditions");
    switch (type) {
      case SEQ_CTRL_TIMER: {
        auto *ctrlCore = (SequenceTimerCore *) (sequenceControl);
        data["control"] = ctrlCore->getControlState().report();
        break;
      }
      case SEQ_CTRL_CRITERIA: {
        auto *ctrlCore = (SequenceCriteriaCore *) (sequenceControl);
        for (int i = 0; i < ctrlCore->getPreconditionSize(); i++) {
          if (ctrlCore->getPreconditionAt(i)->getType() == PREC_CRITERIA) {
            auto *precCoreAtN = (PrecCriteriaCore *) (ctrlCore->getPreconditionAt(i));
            chainOfPreconditions.add(precCoreAtN->getPreconditionState().report());
          }
          else if (ctrlCore->getPreconditionAt(i)->getType() == PREC_TIMER) {
            auto *precCoreAtN = (PrecTimerCore *) (ctrlCore->getPreconditionAt(i));
            chainOfPreconditions.add(precCoreAtN->getPreconditionState().report());
          }
          else if (ctrlCore->getPreconditionAt(i)->getType() == PREC_RANGE) {
            auto *precCoreAtN = (PrecRangeCore *) (ctrlCore->getPreconditionAt(i));
            chainOfPreconditions.add(precCoreAtN->getPreconditionState().report());
          }
        }
        data["control"] = ctrlCore->getControlState().report();
        break;
      }
      case SEQ_CTRL_RANGE: {
        auto *ctrlCore = (SequenceRangeCore *) (sequenceControl);
        for (int i = 0; i < ctrlCore->getPreconditionSize(); i++) {
          if (ctrlCore->getPreconditionAt(i)->getType() == PREC_CRITERIA) {
            auto *precCoreAtN = (PrecCriteriaCore *) (ctrlCore->getPreconditionAt(i));
            chainOfPreconditions.add(precCoreAtN->getPreconditionState().report());
          }
          else if (ctrlCore->getPreconditionAt(i)->getType() == PREC_TIMER) {
            auto *precCoreAtN = (PrecTimerCore *) (ctrlCore->getPreconditionAt(i));
            chainOfPreconditions.add(precCoreAtN->getPreconditionState().report());
          }
          else if (ctrlCore->getPreconditionAt(i)->getType() == PREC_RANGE) {
            auto *precCoreAtN = (PrecRangeCore *) (ctrlCore->getPreconditionAt(i));
            chainOfPreconditions.add(precCoreAtN->getPreconditionState().report());
          }
        }
        data["control"] = ctrlCore->getControlState().report();
        break;
      }
      case SEQ_CTRL_PAR: {
        auto *ctrlCore = (SequenceParCore *) (sequenceControl);
        for (int i = 0; i < ctrlCore->getPreconditionSize(); i++) {
          if (ctrlCore->getPreconditionAt(i)->getType() == PREC_CRITERIA) {
            auto *precCoreAtN = (PrecCriteriaCore *) (ctrlCore->getPreconditionAt(i));
            chainOfPreconditions.add(precCoreAtN->getPreconditionState().report());
          }
          else if (ctrlCore->getPreconditionAt(i)->getType() == PREC_TIMER) {
            auto *precCoreAtN = (PrecTimerCore *) (ctrlCore->getPreconditionAt(i));
            chainOfPreconditions.add(precCoreAtN->getPreconditionState().report());
          }
          else if (ctrlCore->getPreconditionAt(i)->getType() == PREC_RANGE) {
            auto *precCoreAtN = (PrecRangeCore *) (ctrlCore->getPreconditionAt(i));
            chainOfPreconditions.add(precCoreAtN->getPreconditionState().report());
          }
        }
        data["control"] = ctrlCore->getControlState().report();
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
  explicit mutation_sequence_activate() : Mutation("sequence_activate", new permission_sequence_activate) {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    SequenceSchema schema = context->sequence->model->get();
    schema.sequence.isActive = reqData["isActive"];

    int writeOps = context->sequence->model->save(schema);

    SequenceSchema newSchema = context->sequence->model->get();

    DynamicJsonDocument data(1024);
    data["writeOps"] = writeOps;
    data["isActive"] = newSchema.sequence.isActive;

    context->sequence->core->deactivateControls();
    delay(20);
    context->sequence->core->activateControls();
    return data;
  }
};

class mutation_sequence_save : public Mutation {
public:
  explicit mutation_sequence_save() : Mutation("sequence_save", new permission_sequence_save()) {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    SequenceSchema schema = context->sequence->model->get();

    schema.sequence.control.type = SequenceControlStringToEnum(reqData["control"]["type"]);

    // set precondition by providing args
    if (!reqData["preconditions"].isNull()) {
      for (int i = 0; i < reqData["preconditions"].size(); i++) {
        schema.sequence.preconditions[i].type = PreconditionStringToEnum(reqData["preconditions"][i]["type"]);
        schema.sequence.preconditions[i].value = reqData["preconditions"][i]["value"];
      }

      // set precondition to none if providing args size < 3
      for (int i = reqData["preconditions"].size(); i < 3; i++) {
        schema.sequence.preconditions[i].type = PREC_NONE;
        schema.sequence.preconditions[i].value = 0;
      }
    }

    schema.sequence.isActive = false;
    context->sequence->core->deactivateControls();
    int writeOps = context->sequence->model->save(schema);

    SequenceSchema newSchema = context->sequence->model->get();
    SequenceSchema::Sequence sequence = newSchema.sequence;

    DynamicJsonDocument data(1024);
    data["isActive"] = sequence.isActive;
    data["writeOps"] = writeOps;
    JsonObject control = data.createNestedObject("control");
    control["type"] = SequenceControlEnumToString(sequence.control.type);

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

class mutation_sequence_order_save : public Mutation {
public:
  explicit mutation_sequence_order_save() : Mutation("sequence_order_save", new permission_sequence_order_save()) {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    SequenceSchema schema = context->sequence->model->get();
    for (int i = 0; i < reqData["channelOrderAndTiming"].size(); i++) {
      schema.sequence.channelOrderAndTiming[i].channel = reqData["channelOrderAndTiming"][i]["channel"];
      schema.sequence.channelOrderAndTiming[i].workingTimeInSec = reqData["channelOrderAndTiming"][i]["working"];
    }
    schema.sequence.channelOrderAndTimingSize = reqData["channelOrderAndTiming"].size();

    schema.sequence.isActive = false;
    int writeOps = context->sequence->model->save(schema);

    context->sequence->core->deactivateControls();
    delay(20);
    context->sequence->core->activateControls();

    SequenceSchema newSchema = context->sequence->model->get();
    DynamicJsonDocument data(1024);
    data["writeOps"] = writeOps;
    JsonArray channelOrderAndTiming = data.createNestedArray("channelOrderAndTiming");
    for (int i = 0; i < newSchema.sequence.channelOrderAndTimingSize; i++) {
      JsonObject jo = channelOrderAndTiming.createNestedObject();
      jo["working"] = newSchema.sequence.channelOrderAndTiming[i].workingTimeInSec;
      jo["channel"] = newSchema.sequence.channelOrderAndTiming[i].channel;
    }

    return data;
  }
};

#endif //SG_MCU_SEQUENCE_RESOLVERS_H
