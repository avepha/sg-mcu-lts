#include "../init.h"

void q_nsensor_check_correct_type() {
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
  context.nSensorContext->core->updateNSensor(plain_payload, plain_payload_size);

  StaticJsonDocument<64> data;
  data["index"] = 1;
  JsonRequest topic("nsensor", "query", data.as<JsonObject>());
  String result = resolvers.execute(topic.toStaticJsonObject().as<JsonObject>());

  StaticJsonDocument<512> resJson;
  deserializeJson(resJson, result);

  TEST_ASSERT_TRUE(resJson["topic"] == "nsensor");
  TEST_ASSERT_TRUE(resJson["method"] == "query");
  TEST_ASSERT_FALSE(resJson["data"].isNull());
  TEST_ASSERT_TRUE(resJson["data"]["index"] == 1);
  TEST_ASSERT_TRUE(resJson["data"]["isAlive"]);
}

void q_nsensor_RUN_TEST() {
  RUN_TEST(q_nsensor_check_correct_type);
}