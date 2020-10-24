#include <Arduino.h>
#include <vector>
#include "./station.h"
#include "domain/model.h"
#include "domain/sensor/sensor.h"
#include "util/resolveStationTypeEnum.h"
#include "domain/sensor/sensor.h"

#ifndef SG_MCU_STATION_MODEL_H
#define SG_MCU_STATION_MODEL_H

struct StationSchema {
  uint8_t stationSize = 2;
  struct Station {
    STATION_ENUM type;
    bool enable;
    uint8_t address;
    uint8_t sensors[10];
    uint8_t sensorSize;
  } stations[8];
};

class StationModel : public RomModel<StationSchema> {
public:
  StationModel() : RomModel("StationModel") {};

  void initModel() override {
    StationSchema schema;
    schema.stationSize = 2;
    schema.stations[0].type = STATION_GSENSOR;
    schema.stations[0].address = 0x01;
    schema.stations[0].sensorSize = 8;
    schema.stations[0].enable = true;
    uint8_t gsensors[8] = {
        Sensor::SENSORMAP["gs_temperature"],
        Sensor::SENSORMAP["gs_humidity"],
        Sensor::SENSORMAP["gs_vpd"],
        Sensor::SENSORMAP["gs_soilTemperature"],
        Sensor::SENSORMAP["gs_soilMoisture"],
        Sensor::SENSORMAP["gs_co2"],
        Sensor::SENSORMAP["gs_par"],
        Sensor::SENSORMAP["gs_parAccumulation"]
    };
    memcpy(schema.stations[0].sensors, gsensors, sizeof(gsensors));

    schema.stations[1].type = STATION_SOLUTION;
    schema.stations[1].address = 0x10;
    schema.stations[1].sensorSize = 3;
    schema.stations[1].enable = true;
    uint8_t ssensors[3] = {
        Sensor::SENSORMAP["sol_ec"],
        Sensor::SENSORMAP["sol_ph"],
        Sensor::SENSORMAP["sol_waterTemperature"]
    };
    memcpy(schema.stations[1].sensors, ssensors, sizeof(ssensors));

    Log::info("model", "init station model");
    this->save(schema);
  }
};


#endif //SG_MCU_STATION_MODEL_H
