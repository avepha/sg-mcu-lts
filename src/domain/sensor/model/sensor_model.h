// TODO: refactor code -> change location and file name(model.h)
#include "domain/model.h"

#ifndef SG_MCU_SENSOR_MODEL_H
#define SG_MCU_SENSOR_MODEL_H

struct SensorSchema {
  int numberOfSensor = 8;
  char names[12][20] = {
      "temperature",
      "humidity",
      "vpd",
      "soil_temperature",
      "soil",
      "par",
      "par_accumulation",
      "co2",
  };
}; // total size: 240

class SensorModel : public RomModel<SensorSchema> {
public:
  SensorModel() : RomModel("SensorModel") {};
};

#endif //SG_MCU_SENSOR_MODEL_H
