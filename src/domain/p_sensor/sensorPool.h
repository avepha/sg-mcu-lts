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
    sensorsMap[sensor->getSensorId()].push_back(sensor);
  }

private:
  static SensorPool *s_instance;
  SensorPool() = default;

  std::map<std::uint8_t, std::vector<Sensor *>> sensorsMap{}; // <sensorId, sensors[]>
};

SensorPool *SensorPool::s_instance = nullptr;

#endif //SG_MCU_SENSORPOOL_H
