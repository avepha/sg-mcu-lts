#include "../init.h"
#include "../util/nsensor.h"

void q_nsensor_check_correct_type() {
  publishNSensorToStation(1);

  DynamicJsonDocument data(64);
  data["index"] = 1;
  JsonRequest requestTopic("nsensor", "query", data);

  JsonDocument responseJson = resolvers->execute(requestTopic.toJson());

  TEST_ASSERT_TRUE(responseJson["topic"] == "nsensor");
  TEST_ASSERT_TRUE(responseJson["method"] == "query");
  TEST_ASSERT_FALSE(responseJson["data"].isNull());
  TEST_ASSERT_TRUE(responseJson["data"]["index"] == 1);
  TEST_ASSERT_TRUE(responseJson["data"]["isAlive"]);
  TEST_ASSERT_TRUE(responseJson["data"]["sensors"].is<JsonObject>());
}

void q_nsensor_RUN_TEST() {
  RUN_TEST(q_nsensor_check_correct_type);
}