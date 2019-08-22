#include "../init.h"

void q_sensor_check_correct_type() {
  JsonRequest requestTopic("sensor", "query");
  JsonDocument responseJson = resolvers.execute(requestTopic.toJson());

  TEST_ASSERT_TRUE(responseJson["topic"] == "sensor");
  TEST_ASSERT_TRUE(responseJson["method"] == "query");
  TEST_ASSERT_FALSE(responseJson["data"].isNull());
}

void q_sensor_RUN_TEST() {
  RUN_TEST(q_sensor_check_correct_type);
}