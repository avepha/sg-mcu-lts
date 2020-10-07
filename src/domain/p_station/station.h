#include "domain/sensor/sensor.h"
#include "./modbus/modbusPacket.h"

#ifndef SG_MCU_PSTATION_H
#define SG_MCU_PSTATION_H

enum PSTATION_ENUM {
  PSTATION_GSENSOR,
  PSTATION_SOLUTION,
  PSTATION_NULL,
};

class PStation {
public:
  PStation(PSTATION_ENUM _type, uint8_t _address, uint8_t *_sensorIds, uint8_t sensorSize) {
    enable = true;
    type = _type;
    address = _address;
    updatedAtMs = 0;

    byte data[4] = {0x00, 0x00, 0x00, 0x00};
    requestPacket = new ModbusPacket(address, 0x04, data, sizeof(data));
    for (int i = 0; i < sensorSize; i++) {
      sensorMap[_sensorIds[i]] = new Sensor(address, _sensorIds[i]);
      sensorIds.push_back(_sensorIds[i]);
      SensorPool::instance()->addSensor(sensorMap[_sensorIds[i]]);
    }
  }

  PSTATION_ENUM getType() {
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

  void setEnable(bool flag) {
    this->enable = flag;
  }

  bool getEnable() {
    return this->enable;
  }

  void onPacketReceived(const std::vector<byte> &vPacket) {
    //byte address = vPacket[0];
    byte funcCode = vPacket[1];
    uint16_t packetBodySize = vPacket[2];
    byte packetBody[packetBodySize];
    memcpy(packetBody, vPacket.data() + 3, packetBodySize);

    if (funcCode == 0x04) {
      uint16_t sensorSize = packetBodySize / 4;
      float sensorsValue[sensorSize];
      for (uint16_t dataIndex = 0, sensorIndex = 0; sensorIndex < sensorSize; dataIndex += 4, sensorIndex++) {
        memcpy(&sensorsValue[sensorIndex], &packetBody[dataIndex], 4);
      }

      for (int i = 0; i < sensorIds.size(); i++) {
        if (sensorsValue[i] == 0xFFFFFFFF) {
          // set invalid flag to sensor
          sensorMap[sensorIds[i]]->setValid(false);
          Log::warn("station", "invalid sensor: " + String(i));
        }
        else {
          sensorMap[sensorIds[i]]->setValue(sensorsValue[i]);
          sensorMap[sensorIds[i]]->setValid(true);
        }
      }
    }

    // update last seen
    updatedAtMs = millis();
  }

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
  bool enable;
  std::map<uint8_t, Sensor *> sensorMap{};
  std::vector<uint8_t> sensorIds{};
  uint32_t updatedAtMs;

private:
  PSTATION_ENUM type;
  uint8_t address;
  ModbusPacket *requestPacket;
};

#endif //SG_MCU_PSTATION_H
