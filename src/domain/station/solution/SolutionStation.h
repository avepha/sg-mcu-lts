#include <Arduino.h>
#include "../station.h"
#include "../model.h"
#include "logger/log.h"
#include "domain/sensor/sensorPool.h"
#ifndef SG_MCU_SOLUTION_STATION_H
#define SG_MCU_SOLUTION_STATION_H

class SolutionStation : public Station {
public:
  explicit SolutionStation(uint8_t address) : Station(STATION_SOLUTION, address) {
    StationModel model;
    StationSchema::SolutionStation solutionStation = model.get().solutionStation;
    for (int i = 0 ; i < sizeof(solutionStation.SolutionIds) / sizeof(solutionStation.SolutionIds[0]); i++) {
      sensorMap[solutionStation.SolutionIds[i]] = new Sensor(address, solutionStation.SolutionIds[i]);
      sensorIds.push_back(solutionStation.SolutionIds[i]);
      SensorPool::instance()->addSensor(sensorMap[solutionStation.SolutionIds[i]]);
    }
  }

  void onPacketReceived(const std::vector<byte> &vPacket) override {
//    byte address = vPacket[0];
    byte funcCode = vPacket[1];
    uint8_t packetBodySize = vPacket.size() - 4;
    byte packetBody[packetBodySize];
    memcpy(packetBody, vPacket.data() + 2, packetBodySize);

    if (funcCode == 0x04 && vPacket.size() >= 16) {
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
  };
};

#endif //SG_MCU_SOLUTION_STATION_H
