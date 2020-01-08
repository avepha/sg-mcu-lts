#include <Arduino.h>
#include <vector>
#include "domain/model.h"
#include "domain/p_sensor/sensor.h"

#ifndef SG_MCU_STATION_MODEL_H
#define SG_MCU_STATION_MODEL_H

struct StationSchema {
  uint8_t gsensorSize = 1;
  struct GSensorStation {
    uint8_t address;
    uint8_t size;
    uint8_t sensorIds[12];
  } gsensorStations[4] = {
      {
          .address = 0x01,
          .size = 8,
          .sensorIds = {
              Sensor::SENSORMAP["gs_temperature"],
              Sensor::SENSORMAP["gs_humidity"],
              Sensor::SENSORMAP["gs_vpd"],
              Sensor::SENSORMAP["gs_soilTemperature"],
              Sensor::SENSORMAP["gs_soilMoisture"],
              Sensor::SENSORMAP["gs_co2"],
              Sensor::SENSORMAP["gs_par"],
              Sensor::SENSORMAP["gs_parAccumulation"]
          }
      }
  };

  uint8_t solutionSize = 1;
  struct SolutionStation {
    uint8_t address;
    uint8_t size;
    uint8_t sensorIds[4];
  } solutionStations[4] = {
      {
          .address = 0x10,
          .size = 3,
          .sensorIds = {
              Sensor::SENSORMAP["sol_ec"],
              Sensor::SENSORMAP["sol_ph"],
              Sensor::SENSORMAP["sol_waterTemperature"]
          }
      }
  };
};

class StationModel : public RomModel<StationSchema> {
public:
  StationModel() : RomModel("StationModel") {};
};


#endif //SG_MCU_STATION_MODEL_H
