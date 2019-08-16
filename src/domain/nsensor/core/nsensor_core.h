
#include <Arduino.h>
#include "util/converter.h"
#ifndef SG_MCU_NSENSOR_CORE_H
#define SG_MCU_NSENSOR_CORE_H


struct NSensor {
  float sensors[11] = {0};
  unsigned long lastSeen = 0;
};

class NSensorCore {
public:
  NSensorCore() {};

  // bSensors [sta(1 bytes), payload]
  void updateNSensor(byte* bSensors, int size) {
    byte station = bSensors[0];
    byte payload[size - 1];
    memcpy(payload, bSensors + 1, size - 1);

    int sensorBucketSize =  (size - 1) / 2; // 2 bytes per bucket
    float sensorBucket[sensorBucketSize];
    restoreBytesArrayToSensorPayload(payload, sensorBucket, sensorBucketSize);
    for(int i = 0 ; i < sensorBucketSize; i++) {
      nodes[station].sensors[i] = sensorBucket[i];
    }

    nodes[station].lastSeen = millis();
  }

  NSensor* getNSensor() {
    return nodes;
  }

private:
  NSensor nodes[8];
};

#endif //SG_MCU_CORE_H
