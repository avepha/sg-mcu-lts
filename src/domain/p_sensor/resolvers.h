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
      }
    }

    return data;
  };
};

#endif //SG_MCU_STATION_RESOLVERS_H
