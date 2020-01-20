#include <Arduino.h>
#include <vector>
#include "domain/model.h"
#include "domain/p_sensor/sensor.h"

#ifndef SG_MCU_STATION_MODEL_H
#define SG_MCU_STATION_MODEL_H

struct StationSchema {
  struct GSensorStation {
    uint8_t GSensorSensorIds[8] = {
        Sensor::SENSORMAP["gs_temperature"],
        Sensor::SENSORMAP["gs_humidity"],
        Sensor::SENSORMAP["gs_vpd"],
        Sensor::SENSORMAP["gs_soilTemperature"],
        Sensor::SENSORMAP["gs_soilMoisture"],
        Sensor::SENSORMAP["gs_co2"],
        Sensor::SENSORMAP["gs_par"],
        Sensor::SENSORMAP["gs_parAccumulation"]
    };

    uint8_t GsensorSize = 1;
    uint8_t GSensorAddresses[8] = {0x01, 0x03};
  } gSensorStation;

  struct SolutionStation {
    uint8_t SolutionIds[3] = {
        Sensor::SENSORMAP["sol_ec"],
        Sensor::SENSORMAP["sol_ph"],
        Sensor::SENSORMAP["sol_waterTemperature"]
    };

    uint8_t SolutionSize = 1;
    uint8_t SolutionAddresses[8] = {0x10, 0x13};
  } solutionStation;
};

class StationModel : public RomModel<StationSchema> {
public:
  StationModel() : RomModel("StationModel") {};
};


#endif //SG_MCU_STATION_MODEL_H
