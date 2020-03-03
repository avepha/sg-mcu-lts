#include "util/util.h"

#include "domain/mutation.h"
#include "domain/query.h"
#include "validationError.h"
#include "combineContext.h"

#include "./permission.h"

#ifndef SG_MCU_SEQUENCE_PAR_RESOLVERS_H
#define SG_MCU_SEQUENCE_PAR_RESOLVERS_H

// @mutation
class mutation_sequence_par_save : public Mutation {
public:
  explicit mutation_sequence_par_save() : Mutation("sequence_par_save", new permission_sequence_par_save()) {};

  JsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    JsonObject par = reqData["par"];
    SequenceParSchema schema = context->sequencePar->model->get();
    schema.par.parSumInKJ = par["parSumInKJ"];
    schema.par.timing.workingTimeInSecond = par["timing"]["working"];

    int writeOps = context->sequencePar->model->save(schema);
    SequenceParSchema newSchema = context->sequencePar->model->get();
    DynamicJsonDocument data(128);
    data["writeOps"] = writeOps;
    data["par"]["parSumInKJ"] = newSchema.par.parSumInKJ;
    data["par"]["timing"]["working"] = newSchema.par.timing.workingTimeInSecond;

    return data;
  };
};

// @query: date
class query_sequence_par : public Query {
public:
  explicit query_sequence_par() : Query("sequence_par") {};

  JsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    SequenceParSchema schema = context->sequencePar->model->get();
    DynamicJsonDocument data(128);
    data["par"]["parSumInKJ"] = schema.par.parSumInKJ;
    data["par"]["timing"]["working"] = schema.par.timing.workingTimeInSecond;

    return data;
  };
};

#endif //SG_MCU_SEQUENCE_PAR_RESOLVERS_H
