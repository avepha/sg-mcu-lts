#include <array>
#include <Arduino.h>
#include "util/converter.h"

#ifndef SG_MCU_NSENSOR_CORE_H
#define SG_MCU_NSENSOR_CORE_H

struct NSensor {
  std::array<float, 8> sensors{{0, 0, 0, 0, 0, 0, 0, 0}};
  unsigned long timeStamp = 0;
};

class NSensorCore {
public:
  static NSensorCore* instance()
  {
    if (!s_instance)
      s_instance = new NSensorCore;
    return s_instance;
  }

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

    nodes[station].timeStamp = millis();
  }

  std::array<NSensor, 8> getNSensor() {
    return nodes;
  }

  NSensor getAverageSensor() {
    unsigned long currentTimeStamp = millis();
    NSensor averageSensor = {};
    int activeNode = 0;
    for (int i = 0; i < nodes.size(); i++) {
      if (currentTimeStamp - nodes[i].timeStamp >= EXPECTED_ACTIVE_DIFF_TIME) { // 10 seconds
        continue;
      }

      activeNode++;
      for (int j = 0 ; j < averageSensor.sensors.size(); j++) {
        averageSensor.sensors[j] += nodes[i].sensors[j];
      }
    }

    for (int i = 0 ; i < averageSensor.sensors.size(); i++) {
      averageSensor.sensors[i] = averageSensor.sensors[i] / (float)activeNode;
    }

    return averageSensor;
  }

private:
  static NSensorCore *s_instance;
  std::array<NSensor, 8> nodes;
};

NSensorCore *NSensorCore::s_instance = nullptr;

#endif //SG_MCU_CORE_H
