#include <Arduino.h>
#include "../station.h"
#include "../modbus/modbusPacket.h"

#ifndef SG_MCU_GSENSOR_STATION_H
#define SG_MCU_GSENSOR_STATION_H

class GSensorStation : public Station {
public:
  explicit GSensorStation(uint8_t addr) : Station(STATION_GSENSOR, addr) {

  }

  void onPacketReceived(const std::vector<byte> &vPacket) override {
    byte packets[50];
    uint16_t size = vPacket.size();
    memcpy(packets, vPacket.data(), size);

    byte address = packets[0];
    byte funcCode = packets[1];

    if (funcCode == 0x04) { // sensors
      byte *firstDataByte = &packets[2];
      for (uint16_t i = 0 ; i < size; i++) {

      }
    }
  }
};

#endif
