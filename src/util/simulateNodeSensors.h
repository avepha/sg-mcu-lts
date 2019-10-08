#include <TaskScheduler.h>

#ifndef SG_MCU_SIMULATENODESENSORS_H
#define SG_MCU_SIMULATENODESENSORS_H

class SimulateNodeSensorsTask : public Task {
public:
  explicit SimulateNodeSensorsTask(Scheduler *scheduler) : Task(TASK_SECOND, TASK_FOREVER, scheduler, true) {}

  bool Callback() override {
#ifdef SG_TEST
    for (int i = 1; i < 3; i++) { // mock 3 stations
      int bucketSize = 7;
      uint16_t sensorBucket[bucketSize];
      sensorBucket[0] = Float16Compressor::compress((float) random(280, 288) / 10);
      sensorBucket[1] = Float16Compressor::compress((float) random(500, 510) / 10);
      sensorBucket[2] = Float16Compressor::compress((float) random(31, 32) / 10);
      sensorBucket[3] = Float16Compressor::compress((float) random(290, 298) / 10);
      sensorBucket[4] = Float16Compressor::compress((float) random(510, 520) / 10);
      sensorBucket[5] = Float16Compressor::compress((float) random(10, 15) / 10);
      sensorBucket[6] = Float16Compressor::compress((float) random(10, 15) / 10);

      byte payloadSize = bucketSize * sizeof(uint16_t) + sizeof(byte) * 4;
      byte payload_sta_1[payloadSize];
      craftBytesArrayOfSensorPayload(i, sensorBucket, bucketSize, payload_sta_1);

      int plain_payload_size = payloadSize - 3;
      byte plain_payload[plain_payload_size];
      memcpy(plain_payload, payload_sta_1 + 1, plain_payload_size);

      context->nSensorContext->core->updateNSensor(plain_payload, plain_payload_size);
    }

    for (int i = 1; i < 3; i++) { // mock 3 stations
      int bucketSize = 7;
      uint16_t sensorBucket[bucketSize];
      sensorBucket[0] = Float16Compressor::compress((float) random(280, 288) / 10);
      sensorBucket[1] = Float16Compressor::compress((float) random(500, 510) / 10);
      sensorBucket[2] = Float16Compressor::compress((float) random(31, 32) / 10);
      sensorBucket[3] = Float16Compressor::compress((float) random(290, 298) / 10);
      sensorBucket[4] = Float16Compressor::compress((float) random(510, 520) / 10);
      sensorBucket[5] = Float16Compressor::compress((float) random(10, 15) / 10);
      sensorBucket[6] = Float16Compressor::compress((float) random(10, 15) / 10);

      byte payloadSize = bucketSize * sizeof(uint16_t) + sizeof(byte) * 4;
      byte payload_sta_1[payloadSize];
      craftBytesArrayOfSensorPayload(i, sensorBucket, bucketSize, payload_sta_1);

      int plain_payload_size = payloadSize - 3;
      byte plain_payload[plain_payload_size];
      memcpy(plain_payload, payload_sta_1 + 1, plain_payload_size);

      context->nSensorContext->core->updateNSensor(plain_payload, plain_payload_size);
    }
#endif
    return true;
  }
};

#endif //SG_MCU_SIMULATENODESENSORS_H
