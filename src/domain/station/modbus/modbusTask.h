#include <Arduino.h>
#include <TaskScheduler.h>
#include <vector>

#include "./modbusPacket.h"
#include "domain/station/util/resolveStationTypeEnum.h"
#include "./util/generateModbusResponse.h"

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
      if (!isDataComing()) { // always true for SG_TEST
        waitingCycle++;
        if (waitingCycle >= 5) { // request timeout
          Debug::Print("req-timeout, sta " + String(vStations[currentStationIndex]->getAddress()));
          waitingCycle = 0;
          state = WAITING;
          currentStationIndex++;
        }
        return true;
      }
      else { // serial port is available or SG_TEST
        bool isPacketFailed = false;
#ifdef SG_TEST
        std::vector<byte> vBytes = getPacketTestMode(vStations[currentStationIndex]->getAddress());
#else
        std::vector<byte> vBytes = getPacket();
#endif
        byte requestAddress = vStations[currentStationIndex]->getAddress();

        if (vBytes.size() <= 2) {
          Debug::Print("addr: " + String(requestAddress, HEX) + " packet-size 2");
          isPacketFailed = true;
        }

        // step1. check crc
        if (!ModbusPacket::verifyPacket(vBytes)) {
          Debug::Print("res, sta " + String(requestAddress, HEX) + " invalid-crc");
          isPacketFailed = true;
        }

        byte responseAddress = vBytes[0];
        byte responseFuncCode = vBytes[1];

        if (responseFuncCode == 0x08) { // Error handling
          Debug::Print("res, sta " + String(requestAddress, HEX) + " func-code: 0x08");
          isPacketFailed = true;
        }

        if (isPacketFailed) {
          state = WAITING;
          currentStationIndex++;
          return true;
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

      digitalWrite(SG_DIR_PIN, SG_SEND_DIR);
      stationPort.write(requestByte.data(), requestByte.size());
      digitalWrite(SG_DIR_PIN, SG_RECV_DIR);

      Debug::Print(
          "req, sta 0x" + String(vStations[currentStationIndex]->getAddress(), HEX) +
          ", t: " + StationTypeEnumToString(vStations[currentStationIndex]->getType())
      );

      state = REQUESTING;
    }

    return true;
  }

private:
  static bool isDataComing() {
#ifdef SG_TEST
    return true;
#else
    if (stationPort.available() > 0) {
     Debug::Print("Got packet: " + String(stationPort.available()));
    }
    return stationPort.available() != 0;
#endif
  }

  static std::vector<byte> getPacket() {
    std::vector<byte> packets;
    while (stationPort.available()) {
      packets.push_back(stationPort.read());
    }
    return packets;
  }

  static std::vector<byte> getPacketTestMode(byte address) {
    std::vector<byte> packets;
    if (address < 0x10) {
      if (address == 0x03) { // air module is failed
        packets = GenerateModbusResponse::genGsensorPacketWithFailedAirModule(address);
      }
      else {
        packets = GenerateModbusResponse::genGSensorPacket(address);
      }
    }
    else if (address >= 0x01 && address < 0x20) {
      if (address == 0x13) { // all sensor failed
        packets = GenerateModbusResponse::getSolutionPacketWithFailedModule(address);
      }
      else {
        packets = GenerateModbusResponse::getSolutionPacket(address);
      }
    }

    return packets;
  }

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
