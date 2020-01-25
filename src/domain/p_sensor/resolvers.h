#include "domain/query.h"
#include "./permission.h"
#include "./sensorPool.h"

#ifndef SG_MCU_P_SENSOR_RESOLVERS_H
#define SG_MCU_P_SENSOR_RESOLVERS_H

class query_p_sensor : public Query {
public:
  explicit query_p_sensor() : Query("p_sensor", new permission_sensor) {};

  JsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    uint8_t sensorId = reqData["id"];

    SensorPool *sensorPool = SensorPool::instance();
    DynamicJsonDocument data(1024);
    data["id"] = sensorId;
    data["avg"] = sensorPool->getAverageStationBySensorId(sensorId);
    data["available"] = sensorPool->getAvailableStationBySensorId(sensorId);
    data["total"] = sensorPool->getTotalStationBySensorId(sensorId);
    JsonArray stations = data.createNestedArray("stations");
    if (data["total"] > 0) {
      for (const auto& sensor : sensorPool->getSensorMap()[sensorId]) {
        JsonObject staSensor = stations.createNestedObject();
        staSensor["value"] = sensor->getValue();
        staSensor["available"] = sensor->isValid();
        staSensor["station"] = sensor->getStationAddress();
        staSensor["lastSeen"] = millis() - sensor->getUpdatedTime();
        staSensor["outdated"] = sensor->isOutdated();
        staSensor["valid"] = sensor->getValid();
      }
    }

    return data;
  };
};

class query_p_sensors : public Query {
public:
  explicit query_p_sensors() : Query("p_sensors") {};

  JsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    SensorPool *sensorPool = SensorPool::instance();
    DynamicJsonDocument data(1024);
    for (const auto& sensor: sensorPool->getSensorMap()) {
      JsonObject obj = data.createNestedObject();
      obj["id"] = sensor.first;
      obj["available"] = sensorPool->getAvailableStationBySensorId(sensor.first);
      obj["total"] = sensorPool->getTotalStationBySensorId(sensor.first);
    }
    return data;
  };
};


#endif //SG_MCU_STATION_RESOLVERS_H
