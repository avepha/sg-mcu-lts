#include <Arduino.h>
#include "domain/sensor/sensorPool.h"
#include "../station.h"
#include "../modbus/modbusPacket.h"
#include "../model.h"
#include "logger/log.h"

#ifndef SG_MCU_GSOL_STATION_H
#define SG_MCU_GSOL_STATION_H

class GSolutionStation : public Station {
public:
  explicit GSolutionStation(uint8_t addr) : Station(STATION_GSOLUTION, addr) {
    StationModel model;
    StationSchema::GSolutionStation gsolutionSchema = model.get().gSolutionStation;
    for (int i = 0; i < sizeof(gsolutionSchema.sensorIds) / sizeof(gsolutionSchema.sensorIds[0]); i++) {
      sensorMap[gsolutionSchema.sensorIds[i]] = new Sensor(addr, gsolutionSchema.sensorIds[i]);
      sensorIds.push_back(gsolutionSchema.sensorIds[i]);
      SensorPool::instance()->addSensor(sensorMap[gsolutionSchema.sensorIds[i]]);
    }
  }

  void onPacketReceived(const std::vector<byte> &vPacket) override {
//    byte address = vPacket[0];
    byte funcCode = vPacket[1];
    uint16_t packetBodySize = vPacket[2];
    byte packetBody[packetBodySize];
    memcpy(packetBody, vPacket.data() + 3, packetBodySize);
    
    if (funcCode == 0x04 && vPacket.size() >= 33) {
      uint16_t sensorSize = packetBodySize / 4;
      float sensorsValue[sensorSize];
      for (uint16_t dataIndex = 0, sensorIndex = 0; sensorIndex < sensorSize; dataIndex += 4, sensorIndex++) {
        memcpy(&sensorsValue[sensorIndex], &packetBody[dataIndex], 4);
      }

      for (int i = 0; i < sizeof(sensorsValue) / sizeof(sensorsValue[0]); i++) {
        if (sensorsValue[i] == 0xFFFFFFFF) {
          // set invalid flag to sensor
          sensorMap[sensorIds[i]]->setValid(false);
          Log::warn("gsensor", "invalid sensor: " + String(i));
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
