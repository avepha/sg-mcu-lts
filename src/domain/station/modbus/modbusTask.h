#include <Arduino.h>
#include <TaskScheduler.h>
#include <vector>

#include "./modbusPacket.h"
#include "domain/station/util/resolveStationTypeEnum.h"

#ifndef SG_MCU_MODBUSTASK_H
#define SG_MCU_MODBUSTASK_H

class ModbusTask : public Task {
public:
  static ModbusTask *instance() {
    if (!s_instance)
      s_instance = new ModbusTask;
    return s_instance;
  }

  void registerStation(Station *station) {
    vStations.push_back(station);
  }

  bool Callback() override {
    if (state == REQUESTING) {
      if (!stationPort.available()) {
        waitingCycle++;
        if (waitingCycle >= 5) { // request timeout
          Debug::Print("req-timeout, sta " + String(vStations[currentStationIndex]->getAddress()));
          waitingCycle = 0;
          state = WAITING;
          currentStationIndex++;
        }
        return true;
      }
      else { // serial port is available
        std::vector<byte> vBytes;
        while (stationPort.available()) {
          vBytes.push_back(stationPort.read());
        }

        byte requestAddress = vStations[currentStationIndex]->getAddress();

        // step1. check crc
        if (!ModbusPacket::verifyPacket(vBytes)) {
          Debug::Print("addr: " + String(requestAddress, HEX) + " Packet verification failed, Crc is invalid");
          state = WAITING;
          currentStationIndex++;
          return true;
        }

        byte responseAddress = vBytes[0];
        byte responseFuncCode = vBytes[1];

        if (responseFuncCode != 0x08) { // Error handling
          Debug::Print("addr: " + String(requestAddress, HEX) + "Response packet is failed, func-code: 0x08");
        }

        // send to requested station [address(1)][func(1)][data(n * byte)][crc(2)]
        for (auto const &station:vStations) {
          if (station->getAddress() == responseAddress) {
            station->onPacketReceived(vBytes);
          }
        }
        currentStationIndex++;
        state = WAITING;
      }
    }
    else if (state == WAITING) {
      waitingCycle = 0;
      if (vStations.empty()) {
        return true;
      }

      if (currentStationIndex >= vStations.size()) {
        currentStationIndex = 0;
        return true;
      }

      ModbusPacket *requestPacket = vStations[currentStationIndex]->getRequest();
      std::vector<byte> requestByte = requestPacket->getVectorPacket();
      stationPort.write(requestByte.data(), requestByte.size());

      Debug::Print(
          "req, sta 0x" + String(vStations[currentStationIndex]->getAddress(), HEX) +
          ", t: " + StationTypeEnumToString(vStations[currentStationIndex]->getType())
      );

      state = REQUESTING;
    }

    return true;
  }

private:
  static ModbusTask *s_instance;

  ModbusTask() : Task(0, TASK_FOREVER, &gpioScheduler, false) {
    setInterval(intervalTime);
  }

  uint16_t waitingCycle = 0;
  uint16_t intervalTime = 100;
  uint16_t currentStationIndex = 0;
  enum State {
    REQUESTING,
    WAITING,
  } state = WAITING;
  ModbusPacket *packet = nullptr;
  std::vector<Station *> vStations{};
};

ModbusTask *ModbusTask::s_instance = nullptr;
#endif
