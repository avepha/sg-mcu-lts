#include "../init.h"

void q_criteria_with_timer_param_check_correct_type() {
  DynamicJsonDocument data(256);
  data["index"] = 1;
  JsonRequest requestTopic("criteria", "query", data);
  JsonDocument responseJson = resolvers->execute(requestTopic.toJson());

  TEST_ASSERT_TRUE(responseJson["topic"] == "criteria");
  TEST_ASSERT_TRUE(responseJson["method"] == "query");
  TEST_ASSERT_TRUE(responseJson["data"]["index"] == 1);
  TEST_ASSERT_FALSE(responseJson["data"]["criteria"]["sensor"].isNull());
  TEST_ASSERT_FALSE(responseJson["data"]["criteria"]["criteria"].isNull());
  TEST_ASSERT_FALSE(responseJson["data"]["criteria"]["greater"].isNull());
}

void q_criteria_index_is_not_specified() {
  DynamicJsonDocument data(256);
  JsonRequest requestTopic("criteria", "query", data);
  JsonDocument responseJson = resolvers->execute(requestTopic.toJson());

  TEST_ASSERT_TRUE(responseJson["topic"] == "Error");
  TEST_ASSERT_TRUE(responseJson["code"] == "invalid-input");
  TEST_ASSERT_TRUE(responseJson["message"] == "index is not specified.");
}

void q_criteria_index_out_of_range() {
  DynamicJsonDocument data(256);
  data["index"] = -1;
  JsonRequest requestTopic("criteria", "query", data);
  JsonDocument responseJson = resolvers->execute(requestTopic.toJson());

  TEST_ASSERT_TRUE(responseJson["topic"] == "Error");
  TEST_ASSERT_TRUE(responseJson["code"] == "invalid-input");
  TEST_ASSERT_TRUE(responseJson["message"] == "index out of range.");
}

void q_criteria_RUN_TEST() {
  RUN_TEST(q_criteria_with_timer_param_check_correct_type);
  RUN_TEST(q_criteria_index_is_not_specified);
  RUN_TEST(q_criteria_index_out_of_range);
}
