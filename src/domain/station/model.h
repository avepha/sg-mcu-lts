#include <Arduino.h>
#include <vector>
#include "domain/model.h"

#ifndef SG_MCU_STATION_MODEL_H
#define SG_MCU_STATION_MODEL_H

struct StationSchema {
  uint8_t gsensorSize = 1;
  struct GSensorStation {
    uint8_t address;
  } gsensorStations[4] = {{0x01}};

  uint8_t solutionSize = 1;
  struct SolutionStation {
    uint8_t address;
  } solutionStations[4] = {{0x10}};
};

class StationModel : public RomModel<StationSchema> {
public:
  StationModel() : RomModel("StationModel") {};
};


#endif //SG_MCU_STATION_MODEL_H
