#include <Arduino.h>
#include "../station.h"

#ifndef SG_MCU_SOLUTION_STATION_H
#define SG_MCU_SOLUTION_STATION_H

class SolutionStation : public Station {
public:
  explicit SolutionStation(uint8_t address) : Station(STATION_SOLUTION, address) {}
  void onPacketReceived(const std::vector<byte> &vPacket) override {

  };
};

#endif //SG_MCU_SOLUTION_STATION_H
