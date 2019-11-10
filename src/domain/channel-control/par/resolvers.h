#include "domain/mutation.h"
#include "domain/query.h"
#include "util/util.h"

#include "validationError.h"
#include "combineContext.h"

#include "./permission.h"

#ifndef SG_MCU_PAR_RESOLVERS_H
#define SG_MCU_PAR_RESOLVERS_H

// @mutation
class mutation_par_save : public Mutation {
public:
  explicit mutation_par_save() : Mutation("par_save", new permission_par_save()) {};

  JsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    uint8_t index = reqData["index"];
    JsonObject par = reqData["par"];

    ParSchema schema = context->par->model->get();
    schema.pars[index].parSumInKJ = par["parSumInKJ"];
    schema.pars[index].timing.workingTimeInSecond = par["timing"]["working"];

    int writeOps = context->par->model->save(schema);
    ParSchema newSchema = context->par->model->get();
    DynamicJsonDocument data(128);
    data["writeOps"] = writeOps;
    data["index"] = index;
    data["par"]["parSumInJK"] = newSchema.pars[index].parSumInKJ;
    data["par"]["timing"]["working"] = newSchema.pars[index].timing.workingTimeInSecond;

    return data;
  };
};

class mutation_par_sensor_index_save : public Mutation {
public:
  explicit mutation_par_sensor_index_save() : Mutation("par_sensor_index_save", new permission_par_sensor_index_save()) {};

  JsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    ParSchema schema = context->par->model->get();
    schema.parSensorIndex = reqData["parSensorIndex"];

    int writeOps = context->par->model->save(schema);

    ParSchema newSchema = context->par->model->get();
    DynamicJsonDocument data(128);
    data["writeOps"] = writeOps;
    data["parSensorIndex"] = newSchema.parSensorIndex;

    return data;
  };
};

// @query
class query_par_sensor_index : public Query {
public:
  explicit query_par_sensor_index() : Query("par_sensor_index") {};

  JsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    ParSchema schema = context->par->model->get();
    DynamicJsonDocument data(128);
    data["parSensorIndex"] = schema.parSensorIndex;

    return data;
  };
};

class query_par : public Query {
public:
  explicit query_par() : Query("par", new permission_par()) {};

  JsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    int index = reqData["index"];
    ParSchema schema = context->par->model->get();
    DynamicJsonDocument data(128);
    data["index"] = index;
    data["par"]["parSumInJK"] = schema.pars[index].parSumInKJ;
    data["par"]["timing"]["working"] = schema.pars[index].timing.workingTimeInSecond;

    return data;
  };
};

#endif //SG_MCU_RESOLVERS_H
