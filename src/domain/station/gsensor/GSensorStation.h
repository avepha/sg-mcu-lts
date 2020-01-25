#include <Arduino.h>
#include "domain/sensor/sensorPool.h"
#include "../station.h"
#include "../modbus/modbusPacket.h"
#include "../model.h"

#ifndef SG_MCU_GSENSOR_STATION_H
#define SG_MCU_GSENSOR_STATION_H

class GSensorStation : public Station {
public:
  explicit GSensorStation(uint8_t addr) : Station(STATION_GSENSOR, addr) {
    StationModel model;
    StationSchema::GSensorStation gSensorStation = model.get().gSensorStation;
    for (int i = 0; i < sizeof(gSensorStation.GSensorSensorIds) / sizeof(gSensorStation.GSensorSensorIds[0]); i++) {
      sensorMap[gSensorStation.GSensorSensorIds[i]] = new Sensor(addr, gSensorStation.GSensorSensorIds[i]);
      sensorIds.push_back(gSensorStation.GSensorSensorIds[i]);
      SensorPool::instance()->addSensor(sensorMap[gSensorStation.GSensorSensorIds[i]]);
    }
  }

  void onPacketReceived(const std::vector<byte> &vPacket) override {
//    byte address = vPacket[0];
    byte funcCode = vPacket[1];
    uint8_t dataSize = vPacket.size() - 4;
    byte data[dataSize];
    memcpy(data, vPacket.data() + 2, dataSize);

    if (funcCode == 0x04 && vPacket.size() >= 36) {
      float sensorsValue[dataSize / 4];
      for (uint16_t dataIndex = 0, sensorIndex = 0; dataIndex < sizeof(data); dataIndex += 4, sensorIndex++) {
        memcpy(&sensorsValue[sensorIndex], &data[dataIndex], 4);
      }

      for (int i = 0; i < sizeof(sensorsValue) / sizeof(sensorsValue[0]); i++) {
        if (sensorsValue[i] == 0xFFFFFFFF) {
          // set invalid flag to sensor
          sensorMap[sensorIds[i]]->setValid(false);
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
};

#endif
