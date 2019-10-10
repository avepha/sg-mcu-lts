#include "../init.h"

#ifndef SG_MCU_UTIL_NSENSOR_H
#define SG_MCU_UTIL_NSENSOR_H

void publishNSensorToStation(int station) {
  int bucketSize = 6;
  uint16_t sensorBucket[bucketSize];
  sensorBucket[0] = Float16Compressor::compress((float) random(280, 288) / 10);
  sensorBucket[1] = Float16Compressor::compress((float) random(500, 510) / 10);
  sensorBucket[2] = Float16Compressor::compress((float) random(290, 298) / 10);
  sensorBucket[3] = Float16Compressor::compress((float) random(510, 520) / 10);
  sensorBucket[4] = Float16Compressor::compress((float) random(10, 15) / 10);
  sensorBucket[5] = Float16Compressor::compress((float) random(10, 15) / 10);

  byte payloadSize = bucketSize * sizeof(uint16_t) + sizeof(byte) * 4;
  byte payload_sta_n[payloadSize];
  craftBytesArrayOfSensorPayload(station, sensorBucket, bucketSize, payload_sta_n);

  int plain_payload_size = payloadSize - 3;
  byte plain_payload[plain_payload_size];
  memcpy(plain_payload, payload_sta_n + 1, plain_payload_size);

  context->nsensors->core->updateNSensor(plain_payload, plain_payload_size);
}
#endif
