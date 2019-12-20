#include <Arduino.h>
#include "../station.h"

#ifndef SG_MCU_GSENSOR_STATION_H
#define SG_MCU_GSENSOR_STATION_H

class GSensorStation : public Station {
public:
  explicit GSensorStation(uint8_t addr) : Station(STATION_GSENSOR, addr) {}
  void onPacketReceived(const std::vector<byte> &vPacket) override {
    
  }
};

#endif
