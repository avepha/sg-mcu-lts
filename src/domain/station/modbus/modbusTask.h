#include <Arduino.h>
#include <TaskScheduler.h>
#include <vector>

#include "./modbusPacket.h"
#include "domain/station/util/resolveStationTypeEnum.h"
#include "./util/generateModbusResponse.h"
#include "logger/log.h"

#ifndef SG_MCU_MODBUSTASK_H
#define SG_MCU_MODBUSTASK_H


// TODO: move this to appropriate location
static void sendDataToStation(byte *data, int size) {
  digitalWrite(RS485_DIR_PIN, RS485_SEND_MODE);
  delay(10);
  stationPort.write(data, size);
  delay(10);
  digitalWrite(RS485_DIR_PIN, RS485_RECV_MODE);
}

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
    switch(state) {
      case WAITING_RESPONSE: {
        if (isDataComing()) {
          bool isPacketFailed = false;
#ifdef SG_TEST
          std::vector<byte> vBytes = getPacketTestMode(vStations[currentStationIndex]->getAddress());
#else
          std::vector<byte> vBytes = getPacket();
#endif

          String strPacket = "";
          for (int i = 0; i < vBytes.size(); i++)
            strPacket += String(vBytes[i], HEX) + " ";
          Log::trace("modbus", "res, packet: " + strPacket);

          byte requestAddress = vStations[currentStationIndex]->getAddress();

          if (vBytes.size() <= 2) {
            Log::error("modbus", "res, sta 0x" + String(requestAddress, HEX) + " error-packet-size-2");
            isPacketFailed = true;
          }

          // step1. check crc
          if (!ModbusPacket::verifyPacket(vBytes)) {
            Log::error("modbus", "res, sta 0x" + String(requestAddress, HEX) + " error-invalid-crc");
            isPacketFailed = true;
          }

          byte responseAddress = vBytes[0];
          byte responseFuncCode = vBytes[1];

          if (responseFuncCode == 0x08) { // Error handling
            Log::error("modbus", "res, sta 0x" + String(requestAddress, HEX) + " error-func-code: 0x08");
            isPacketFailed = true;
          }

          if (isPacketFailed) {
            state = REQUEST_NEXT;
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
          state = REQUEST_NEXT;
        }
        else {
          waitingCycle++;
          if (waitingCycle >= 10) { // request timeout
            Log::error("modbus", "req-timeout, sta " + String(vStations[currentStationIndex]->getAddress()));
            waitingCycle = 0;
            state = REQUEST_NEXT;
            currentStationIndex++;
          }
          return true;
        }
        break;
      }
      case REQUEST_NEXT: {
        waitingCycle = 0;
        if (vStations.empty()) {
          return true;
        }

        if (millis() - lastSendTs <= 1000) {
          return true;
        }

        if (currentStationIndex >= vStations.size()) {
          currentStationIndex = 0;
          return true;
        }

        ModbusPacket *requestPacket = vStations[currentStationIndex]->getRequest();
        std::vector<byte> requestByte = requestPacket->getVectorPacket();

        sendDataToStation(requestByte.data(), requestByte.size());
        lastSendTs = millis();

        Log::trace("modbus",
            "req, sta 0x" + String(vStations[currentStationIndex]->getAddress(), HEX) +
            ", t: " + StationTypeEnumToString(vStations[currentStationIndex]->getType())
        );

        state = WAITING_RESPONSE;
        break;
      }
    }
    return true;
  }

private:
  uint32_t lastSendTs = 0;

  static bool isDataComing() {
#ifdef SG_TEST
    return true;
#else
    return stationPort.available() != 0;
#endif
  }

  static std::vector<byte> getPacket() {
    std::vector<byte> packets;

    uint32_t ts = millis();
    while (true) {
      if (stationPort.available()) {
        packets.push_back(stationPort.read());
        ts = millis();
      }
      else {
        if (millis() - ts >= 4) {
          return packets;
        }
      }
    }
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

  ModbusTask() : Task(0, TASK_FOREVER, &backgroundScheduler, false) {
    setInterval(intervalTime);
  }

  uint16_t waitingCycle = 0;
  uint16_t intervalTime = 100;
  uint16_t currentStationIndex = 0;
  enum State {
    WAITING_RESPONSE,
    REQUEST_NEXT,
  } state = REQUEST_NEXT;
  ModbusPacket *packet = nullptr;
  std::vector<Station *> vStations{};

};

ModbusTask *ModbusTask::s_instance = nullptr;
#endif
