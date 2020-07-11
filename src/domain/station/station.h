#include "domain/sensor/sensor.h"
#include "./modbus/modbusPacket.h"

#ifndef SG_MCU_STATION_H
#define SG_MCU_STATION_H

enum STATION_ENUM {
  STATION_GSENSOR,
  STATION_SOLUTION,
  STATION_GSOLUTION,
};

class Station {
public:
  Station(STATION_ENUM _type, uint8_t _address) {
    type = _type;
    address = _address;
    updatedAtMs = 0;

    byte data[4] = {0x00, 0x00, 0x00, 0x00};
    requestPacket = new ModbusPacket(address, 0x04, data, sizeof(data));
  }

  STATION_ENUM getType() {
    return type;
  }

  uint8_t getAddress() {
    return address;
  }

  uint32_t getLastSeen() {
    return millis() - updatedAtMs;
  }

  bool isAvailable() {
    return updatedAtMs != 0 && millis() - updatedAtMs < 1000 * 10; // never updated or 10 seconds
  }

  virtual void onPacketReceived(const std::vector<byte> &vPacket) = 0;

  ModbusPacket *getRequest() {
    return requestPacket;
  }

  std::vector<uint8_t> getSensorIds() {
    return sensorIds;
  }

  std::map<uint8_t, Sensor *> getSensorMap() {
    return sensorMap;
  }

protected:
  std::map<uint8_t, Sensor *> sensorMap{};
  std::vector<uint8_t> sensorIds{};
  uint32_t updatedAtMs;

private:
  STATION_ENUM type;
  uint8_t address;
  ModbusPacket *requestPacket;
};

#endif //SG_MCU_STATION_H
