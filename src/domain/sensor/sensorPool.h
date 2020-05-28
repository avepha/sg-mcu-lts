#include <Arduino.h>
#include "./sensor.h"

#ifndef SG_MCU_SENSOR_SENSORPOOL_H
#define SG_MCU_SENSOR_SENSORPOOL_H

class SensorPool {
public:
  static SensorPool *instance() {
    if (!s_instance)
      s_instance = new SensorPool;
    return s_instance;
  }

  void addSensor(Sensor *sensor) {
    mSensorIdToSensors[sensor->getSensorId()].push_back(sensor);
  }

  std::map<std::uint8_t, std::vector<Sensor *>> getSensorMap() {
    return mSensorIdToSensors;
  }

  uint16_t getAvailableStationBySensorId(uint8_t sensorId) {
    uint16_t available = 0;
    for (const auto &sensor : mSensorIdToSensors[sensorId]) {
      if (sensor->isValid()) {
        available++;
      }
    }

    return available;
  }

  uint16_t getTotalStationBySensorId(uint8_t sensorId) {
    return mSensorIdToSensors[sensorId].size();
  }

  float getAverageStationBySensorId(uint8_t sensorId) {
    float sum = 0;
    float available = 0;
    for (const auto &sensor : mSensorIdToSensors[sensorId]) {
      if (sensor->isValid()) {
        available++;
        sum += sensor->getValue();
      }
    }

    if (!available) {
      return (float)0xFFFFFFFF;
    }

    return sum / available;
  }

private:
  static SensorPool *s_instance;

  SensorPool() = default;

  std::map<std::uint8_t, std::vector<Sensor *>> mSensorIdToSensors{}; // <sensorId, sensors[]>
};

SensorPool *SensorPool::s_instance = nullptr;

#endif //SG_MCU_SENSORPOOL_H
