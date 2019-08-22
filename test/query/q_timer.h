#include "../init.h"

void q_timer_index_is_not_defined() {
  DynamicJsonDocument data(64);
  JsonRequest requestTopic("timer", "query", data);
  JsonDocument responseJson = resolvers.execute(requestTopic.toJson());

  TEST_ASSERT_TRUE(responseJson["topic"] == "Error");
  TEST_ASSERT_TRUE(responseJson["code"] == "invalid-input");
  TEST_ASSERT_TRUE(responseJson["message"] == "index is not specified.");
}

void q_timer_index_out_of_range() {
  DynamicJsonDocument data(64);
  data["index"] = -1;
  JsonRequest requestTopic("timer", "query", data);
  JsonDocument responseJson = resolvers.execute(requestTopic.toJson());

  TEST_ASSERT_TRUE(responseJson["topic"] == "Error");
  TEST_ASSERT_TRUE(responseJson["code"] == "invalid-input");
  TEST_ASSERT_TRUE(responseJson["message"] == "index out of range.");
}

void q_timer_check_correct_type() {
  DynamicJsonDocument data(64);
  data["type"] = "timer";
  data["index"] = 0;
  JsonRequest requestTopic("timer", "query", data);
  JsonDocument responseJson = resolvers.execute(requestTopic.toJson());

  TEST_ASSERT_TRUE(responseJson["topic"] == "timer");
  TEST_ASSERT_TRUE(responseJson["method"] == "query");
  TEST_ASSERT_TRUE(responseJson["data"]["index"] == 0);
  TEST_ASSERT_TRUE(!responseJson["data"]["timers"].isNull());
}

void q_timer_RUN_TEST() {
  RUN_TEST(q_timer_index_is_not_defined);
  RUN_TEST(q_timer_index_out_of_range);
  RUN_TEST(q_timer_check_correct_type);
}