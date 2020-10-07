#include "domain/query.h"
#include "./permission.h"
#include "./util/resolveStationTypeEnum.h"

#ifndef SG_MCU_PSTATION_RESOLVERS_H
#define SG_MCU_PSTATION_RESOLVERS_H

class query_pstations : public Query {
public:
  explicit query_pstations() : Query("pstations") {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    DynamicJsonDocument data(1024);
    PStationSchema pStationSchema = context->pstation->model->get();
    JsonArray stations = data.createNestedArray();
    for (int i = 0; i < pStationSchema.stationSize; i++) {
      JsonObject object = stations.createNestedObject();
      object["type"] = PStationTypeEnumToString(pStationSchema.stations[i].type);
      object["address"] = pStationSchema.stations[i].address;
      object["enable"] = pStationSchema.stations[i].enable;
      JsonArray sensors = object.createNestedArray("sensors");
      for (int j = 0; j < pStationSchema.stations[i].sensorSize; j++) {
        sensors.add(pStationSchema.stations[i].sensors[j]);
      }
    }
    return data;
  }
};

class mutation_pstation_remove : public Mutation {
public:
  explicit mutation_pstation_remove() : Mutation("pstation_remove", new permission_pstation_remove) {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    uint8_t address = reqData["address"];
    PStationSchema model = context->pstation->model->get();
    // find index
    uint8_t index = 0;
    for (int i = 0; i < model.stationSize; i++) {
      if (model.stations[i].address == address) break;
      index++;
    }

    // change to vector
    std::vector<PStationSchema::Station> vStation(std::begin(model.stations), std::end(model.stations));

    //remove index
    vStation.erase(vStation.begin() + index);
    std::copy(vStation.begin(), vStation.end(), model.stations);
    model.stationSize--;
    context->pstation->model->save(model);

    DynamicJsonDocument data(1024);
    PStationSchema newModel = context->pstation->model->get();
    JsonArray stations = data.createNestedArray();
    for (int i = 0; i < newModel.stationSize; i++) {
      JsonObject object = stations.createNestedObject();
      object["type"] = PStationTypeEnumToString(newModel.stations[i].type);
      object["address"] = newModel.stations[i].address;
      object["enable"] = newModel.stations[i].enable;
      JsonArray sensors = object.createNestedArray("sensors");
      for (int j = 0; j < newModel.stations[i].sensorSize; j++) {
        sensors.add(newModel.stations[i].sensors[j]);
      }
    }
    return data;
  }
};

class mutation_pstation_add : public Mutation {
public:
  explicit mutation_pstation_add() : Mutation("pstation_add", new permission_pstation_add) {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    uint8_t address = reqData["address"];
    PStationSchema model = context->pstation->model->get();

    // find index
    uint8_t index;
    for (index = 0; index < model.stationSize; index++) {
      if (model.stations[index].address == address) break;
    }

    model.stations[index].type = PStationTypeStringToEnum(reqData["type"]);
    model.stations[index].address = address;
    model.stations[index].enable = reqData["enable"];
    model.stations[index].sensorSize = reqData["sensors"].size();
    for (int i = 0; i < model.stations[index].sensorSize; i++) {
      model.stations[index].sensors[i] = reqData["sensors"][i];
    }

    //not found
    if (index >= model.stationSize) {
      model.stationSize = model.stationSize + 1;
    }
    context->pstation->model->save(model);

    DynamicJsonDocument data(1024);
    PStationSchema newModel = context->pstation->model->get();
    JsonArray stations = data.createNestedArray();
    for (int i = 0; i < newModel.stationSize; i++) {
      JsonObject object = stations.createNestedObject();
      object["type"] = PStationTypeEnumToString(newModel.stations[i].type);
      object["address"] = newModel.stations[i].address;
      object["enable"] = newModel.stations[i].enable;
      JsonArray sensors = object.createNestedArray("sensors");
      for (int j = 0; j < newModel.stations[i].sensorSize; j++) {
        sensors.add(newModel.stations[i].sensors[j]);
      }
    }
    return data;
  }
};

#endif //SG_MCU_PSTATION_RESOLVERS_H
