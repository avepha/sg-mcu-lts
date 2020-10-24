#include "domain/query.h"
#include "./permission.h"
#include "./util/resolveStationTypeEnum.h"

#ifndef SG_MCU_STATION_RESOLVERS_H
#define SG_MCU_STATION_RESOLVERS_H

class query_station_data : public Query {
public:
  explicit query_station_data() : Query("station_data") {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    DynamicJsonDocument data(1024);
    StationSchema stationSchema = context->station->model->get();
    JsonArray stations = data.createNestedArray();
    for (int i = 0; i < stationSchema.stationSize; i++) {
      JsonObject object = stations.createNestedObject();
      object["type"] = StationTypeEnumToString(stationSchema.stations[i].type);
      object["address"] = stationSchema.stations[i].address;
      object["enable"] = stationSchema.stations[i].enable;
      JsonArray sensors = object.createNestedArray("sensors");
      for (int j = 0; j < stationSchema.stations[i].sensorSize; j++) {
        sensors.add(stationSchema.stations[i].sensors[j]);
      }
    }
    return data;
  }
};

class query_stations : public Query {
public:
  explicit query_stations() : Query("stations") {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    std::vector<Station*> stations = context->station->core->getStations();
    DynamicJsonDocument data(1024);
    for (int i = 0 ; i < stations.size(); i++) {
      JsonObject stationObject = data.createNestedObject();
      stationObject["type"] = StationTypeEnumToString(stations[i]->getType());
      stationObject["address"] = stations[i]->getAddress();
      stationObject["lastSeen"] = stations[i]->getLastSeen();
      stationObject["available"] = stations[i]->isAvailable();
    }
    return data;
  }
};

class query_station : public Query {
public:
  explicit query_station() : Query("station", new permission_station) {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    std::vector<Station*> stations = context->station->core->getStations();
    uint8_t address = reqData["address"];

    DynamicJsonDocument data(1024);
    for(auto station: stations) {
      if (station->getAddress() == address) {
        data["type"] = StationTypeEnumToString(station->getType());
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
  explicit mutation_station_data_remove() : Mutation("station_data_remove", new permission_station_remove) {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    uint8_t address = reqData["address"];
    StationSchema model = context->station->model->get();
    // find index
    uint8_t index = 0;
    for (int i = 0; i < model.stationSize; i++) {
      if (model.stations[i].address == address) break;
      index++;
    }

    // change to vector
    std::vector<StationSchema::Station> vStation(std::begin(model.stations), std::end(model.stations));

    //remove index
    vStation.erase(vStation.begin() + index);
    std::copy(vStation.begin(), vStation.end(), model.stations);
    model.stationSize--;
    context->station->model->save(model);

    DynamicJsonDocument data(1024);
    StationSchema newModel = context->station->model->get();
    JsonArray stations = data.createNestedArray();
    for (int i = 0; i < newModel.stationSize; i++) {
      JsonObject object = stations.createNestedObject();
      object["type"] = StationTypeEnumToString(newModel.stations[i].type);
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
  explicit mutation_station_data_add() : Mutation("station_data_add", new permission_station_add) {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    uint8_t address = reqData["address"];
    StationSchema model = context->station->model->get();

    // find index
    uint8_t index;
    for (index = 0; index < model.stationSize; index++) {
      if (model.stations[index].address == address) break;
    }

    model.stations[index].type = StationTypeStringToEnum(reqData["type"]);
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
    context->station->model->save(model);

    DynamicJsonDocument data(1024);
    StationSchema newModel = context->station->model->get();
    JsonArray stations = data.createNestedArray();
    for (int i = 0; i < newModel.stationSize; i++) {
      JsonObject object = stations.createNestedObject();
      object["type"] = StationTypeEnumToString(newModel.stations[i].type);
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

#endif //SG_MCU_STATION_RESOLVERS_H
