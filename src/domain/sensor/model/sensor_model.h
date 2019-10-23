// TODO: refactor code -> change location and file name(model.h)
#include "domain/model.h"

#ifndef SG_MCU_SENSOR_MODEL_H
#define SG_MCU_SENSOR_MODEL_H

const int SENSOR_ROM_ADDR = 0;

struct SensorSchema {
  int numberOfSensor = 9;
  char names[12][20] = {
      "temperature",
      "humidity",
      "vpd",
      "soil_temperature",
      "soil",
      "soil_potential",
      "co2",
      "par",
      "par_accumulation",
      "ec",
      "ph",
      "no2",
  };
}; // total size: 240

class SensorModel : public RomModel<SensorSchema> {
public:
  SensorModel() : RomModel(SENSOR_ROM_ADDR, "SensorModel") {};
};

#endif //SG_MCU_SENSOR_MODEL_H
