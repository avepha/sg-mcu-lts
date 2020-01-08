#include <Arduino.h>
#include <map>

#ifndef SG_MCU_SENSOR_ENTITY_H
#define SG_MCU_SENSOR_ENTITY_H

class Sensor {
public:
  static std::map<std::string, uint8_t> SENSORMAP;

  Sensor() : stationAddr(-1), sensorId(-1), value(-1), isValid(false) {}

  uint8_t getStationAddress() {
    return stationAddr;
  }

  uint32_t getValue() {
    return value;
  }

private:
  uint8_t stationAddr;
  uint8_t sensorId;
  uint32_t value;
  bool isValid;
};

std::map<std::string, uint8_t> Sensor::SENSORMAP{
    {"gs_temperature",       0x01},
    {"gs_humidity",          0x02},
    {"gs_vpd",               0x03},
    {"gs_soilTemperature",   0x04},
    {"gs_soilMoisture",      0x05},
    {"gs_soilPotential",     0x06},
    {"gs_co2",               0x06},
    {"gs_par",               0x06},
    {"gs_parAccumulation",   0x06},
    {"sol_ec",               0x06},
    {"sol_ph",               0x06},
    {"sol_waterTemperature", 0x06},
};

#endif //SG_MCU_SENSOR_H
