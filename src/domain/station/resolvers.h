#include "domain/query.h"
#include "./permission.h"
#include "domain/station/util/resolveStationTypeEnum.h"
#ifndef SG_MCU_STATION_RESOLVERS_H
#define SG_MCU_STATION_RESOLVERS_H

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
  };
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
  };
};

#endif //SG_MCU_STATION_RESOLVERS_H
