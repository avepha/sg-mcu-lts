#include <Arduino.h>
#include <map>

#ifndef SG_MCU_SENSOR_ENTITY_H
#define SG_MCU_SENSOR_ENTITY_H

class Sensor {
public:
  static std::map<std::string, uint8_t> SENSORMAP;

  Sensor(uint8_t addr, uint8_t sensorId) : stationAddr(addr), sensorId(sensorId), value(-1), valid(false) {}

  void setValue(float val) {
    value = val;
  }

  void setValid(uint32_t val) {
    valid = val;
  }

  uint8_t getStationAddress() {
    return stationAddr;
  }

  uint8_t getSensorId() {
    return sensorId;
  }

  float getValue() {
    return value;
  }

  bool isValid() {
    return valid;
  }

private:
  uint8_t stationAddr;
  uint8_t sensorId;
  float value;
  bool valid;
};

std::map<std::string, uint8_t> Sensor::SENSORMAP{
    {"gs_temperature",       0x01},
    {"gs_humidity",          0x02},
    {"gs_vpd",               0x03},
    {"gs_soilTemperature",   0x04},
    {"gs_soilMoisture",      0x05},
    {"gs_soilPotential",     0x06},
    {"gs_co2",               0x07},
    {"gs_par",               0x08},
    {"gs_parAccumulation",   0x09},
    {"sol_ec",               0x0A},
    {"sol_ph",               0x0B},
    {"sol_waterTemperature", 0x0C},
};

#endif //SG_MCU_SENSOR_H
