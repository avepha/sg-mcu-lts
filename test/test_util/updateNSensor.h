#include "../init.h"
#include "util/converter.h"

void util_createAndRestorePayload() {
  int bucketSize = 7;
  uint16_t sensorBucket[bucketSize];
  sensorBucket[0] = Float16Compressor::compress((float) random(280, 288) / 10);
  sensorBucket[1] = Float16Compressor::compress((float) random(500, 510) / 10);
  sensorBucket[2] = Float16Compressor::compress((float) random(290, 298) / 10);
  sensorBucket[3] = Float16Compressor::compress((float) random(510, 520) / 10);
  sensorBucket[4] = Float16Compressor::compress((float) random(10, 15) / 10);
  sensorBucket[5] = Float16Compressor::compress((float) random(10, 15) / 10);
  sensorBucket[6] = Float16Compressor::compress((float) random(710, 720) / 10);

  byte payloadSize = bucketSize * sizeof(uint16_t) + sizeof(byte) * 4;
  byte payload_sta_1[payloadSize];
  craftBytesArrayOfSensorPayload(1, sensorBucket, bucketSize, payload_sta_1);

  float dePayload[bucketSize];
  restoreBytesArrayToSensorPayload(payload_sta_1 + 2, dePayload, bucketSize);

  TEST_ASSERT_FLOAT_WITHIN(2, 28, dePayload[0]);
  TEST_ASSERT_FLOAT_WITHIN(2, 50, dePayload[1]);
  TEST_ASSERT_FLOAT_WITHIN(2, 29, dePayload[2]);
  TEST_ASSERT_FLOAT_WITHIN(2, 51, dePayload[3]);
  TEST_ASSERT_FLOAT_WITHIN(2, 1, dePayload[4]);
  TEST_ASSERT_FLOAT_WITHIN(2, 1, dePayload[5]);
  TEST_ASSERT_FLOAT_WITHIN(2, 71, dePayload[6]);
}

void util_update_sensor() {
  //create payload
  int bucketSize = 6;
  uint16_t sensorBucket[bucketSize];
  sensorBucket[0] = Float16Compressor::compress((float) random(280, 288) / 10);
  sensorBucket[1] = Float16Compressor::compress((float) random(500, 510) / 10);
  sensorBucket[2] = Float16Compressor::compress((float) random(290, 298) / 10);
  sensorBucket[3] = Float16Compressor::compress((float) random(510, 520) / 10);
  sensorBucket[4] = Float16Compressor::compress((float) random(10, 15) / 10);
  sensorBucket[5] = Float16Compressor::compress((float) random(10, 15) / 10);

  byte payloadSize = bucketSize * sizeof(uint16_t) + sizeof(byte) * 4;
  byte payload_sta_1[payloadSize];
  craftBytesArrayOfSensorPayload(1, sensorBucket, bucketSize, payload_sta_1);

  int plain_payload_size = payloadSize - 3;
  byte plain_payload[plain_payload_size];
  memcpy(plain_payload, payload_sta_1 + 1, plain_payload_size);

  float dePayload[bucketSize];
  restoreBytesArrayToSensorPayload(plain_payload + 1, dePayload, bucketSize);

  TEST_ASSERT_FLOAT_WITHIN(2, 28, dePayload[0]);
  TEST_ASSERT_FLOAT_WITHIN(2, 50, dePayload[1]);
  TEST_ASSERT_FLOAT_WITHIN(2, 29, dePayload[2]);
  TEST_ASSERT_FLOAT_WITHIN(2, 51, dePayload[3]);
  TEST_ASSERT_FLOAT_WITHIN(2, 1, dePayload[4]);
  TEST_ASSERT_FLOAT_WITHIN(2, 1, dePayload[5]);

  context->nSensorContext->core->updateNSensor(plain_payload, plain_payload_size);
  JsonRequest requestTopic("nsensors", "query");
  JsonDocument responseJson = resolvers->execute(requestTopic.toJson());

  TEST_ASSERT_TRUE(responseJson["topic"] == "nsensors");
  TEST_ASSERT_TRUE(responseJson["method"] == "query");
  TEST_ASSERT_FALSE(responseJson["data"][0].isNull());
  TEST_ASSERT_TRUE(responseJson["data"][0]["index"] == 1);
}

void util_updateNSensor_RUN_TEST() {
  RUN_TEST(util_createAndRestorePayload);
  RUN_TEST(util_update_sensor);
}