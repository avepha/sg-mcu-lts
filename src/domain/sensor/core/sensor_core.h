//
// Created by Alfarie-MBP on 2019-08-11.
//

#ifndef SG_MCU_SENSOR_CORE_H
#define SG_MCU_SENSOR_CORE_H

class SensorCore {
public:
  SensorCore() {};

  float* getSensors() {
    return sensors;
  }

private:
  float sensors[11] = {0};
};

#endif //SG_MCU_SENSOR_CORE_H
