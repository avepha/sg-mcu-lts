#include "domain/query.h"
#include "./permission.h"
#include "./util/resolveStationTypeEnum.h"

#ifndef SG_MCU_PSTATION_RESOLVERS_H
#define SG_MCU_PSTATION_RESOLVERS_H

class query_station_data : public Query {
public:
  explicit query_station_data() : Query("station_data") {};

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

class query_pstations : public Query {
public:
  explicit query_pstations() : Query("pstations") {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    std::vector<PStation*> stations = context->pstation->core->getStations();
    DynamicJsonDocument data(1024);
    for (int i = 0 ; i < stations.size(); i++) {
      JsonObject stationObject = data.createNestedObject();
      stationObject["type"] = PStationTypeEnumToString(stations[i]->getType());
      stationObject["address"] = stations[i]->getAddress();
      stationObject["lastSeen"] = stations[i]->getLastSeen();
      stationObject["available"] = stations[i]->isAvailable();
    }
    return data;
  }
};

class query_pstation : public Query {
public:
  explicit query_pstation() : Query("pstation", new permission_pstation) {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    std::vector<PStation*> stations = context->pstation->core->getStations();
    uint8_t address = reqData["address"];

    DynamicJsonDocument data(1024);
    for(auto station: stations) {
      if (station->getAddress() == address) {
        data["type"] = PStationTypeEnumToString(station->getType());
        data["address"] = station->getAddress();
        data["lastSeen"] = station->getLastSeen();
        data["available"] = station->isAvailable();
        JsonArray sensors = data.createNestedArray("sensors");
        for (auto sensorId : station->getSensorIds()) {
          JsonObject sensor = sensors.createNestedObject();
          sensor["id"] = sensorId;
          sensor["isValid"] = station->getSensorMap()[sensorId]->isValid();
          sensor["value"] = (float)station->getSensorMap()[sensorId]->getValue();
        }
        break;
      }
    }
    return data;
  }
};

class mutation_station_data_remove : public Mutation {
public:
  explicit mutation_station_data_remove() : Mutation("station_data_remove", new permission_pstation_remove) {};

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

class mutation_station_data_add : public Mutation {
public:
  explicit mutation_station_data_add() : Mutation("station_data_add", new permission_pstation_add) {};

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
