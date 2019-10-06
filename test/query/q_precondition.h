#include "../init.h"

void q_precondition_type_is_not_defined() {
  DynamicJsonDocument data(64);
  data["index"] = 0;
  JsonRequest requestTopic("precondition", "query", data);
  JsonDocument responseJson = resolvers->execute(requestTopic.toJson());

  TEST_ASSERT_TRUE(responseJson["topic"] == "Error");
  TEST_ASSERT_TRUE(responseJson["code"] == "invalid-input");
  TEST_ASSERT_TRUE(responseJson["message"] == "type Or index field is not specified.");
}

void q_precondition_index_is_not_defined() {
  DynamicJsonDocument data(64);
  data["type"] = "timer";
  JsonRequest requestTopic("precondition", "query", data);
  JsonDocument responseJson = resolvers->execute(requestTopic.toJson());

  TEST_ASSERT_TRUE(responseJson["topic"] == "Error");
  TEST_ASSERT_TRUE(responseJson["code"] == "invalid-input");
  TEST_ASSERT_TRUE(responseJson["message"] == "type Or index field is not specified.");
}

void q_precondition_index_out_of_range() {
  DynamicJsonDocument data(64);
  data["type"] = "timer";
  data["index"] = -1;
  JsonRequest requestTopic("precondition", "query", data);
  JsonDocument responseJson = resolvers->execute(requestTopic.toJson());

  TEST_ASSERT_TRUE(responseJson["topic"] == "Error");
  TEST_ASSERT_TRUE(responseJson["code"] == "invalid-input");
  TEST_ASSERT_TRUE(responseJson["message"] == "index out of range.");
}

void q_precondition_no_match_type() {
  DynamicJsonDocument data(64);
  data["type"] = "nomatch";
  data["index"] = 0;
  JsonRequest requestTopic("precondition", "query", data);
  JsonDocument responseJson = resolvers->execute(requestTopic.toJson());

  TEST_ASSERT_TRUE(responseJson["topic"] == "Error");
  TEST_ASSERT_TRUE(responseJson["code"] == "invalid-input");
  TEST_ASSERT_TRUE(responseJson["message"] == "No such the provided type.");
}

void q_precondition_with_timer_param_check_correct_type() {
  DynamicJsonDocument data(64);
  data["type"] = "timer";
  data["index"] = 0;
  JsonRequest requestTopic("precondition", "query", data);
  JsonDocument responseJson = resolvers->execute(requestTopic.toJson());

  TEST_ASSERT_TRUE(responseJson["topic"] == "precondition");
  TEST_ASSERT_TRUE(responseJson["method"] == "query");
  TEST_ASSERT_TRUE(responseJson["data"]["index"] == 0);
  TEST_ASSERT_TRUE(!responseJson["data"]["timers"].isNull());
}

void q_precondition_with_criteria_param_check_correct_type() {
  DynamicJsonDocument data(64);
  data["type"] = "criteria";
  data["index"] = 0;
  JsonRequest requestTopic("precondition", "query", data);
  JsonDocument responseJson = resolvers->execute(requestTopic.toJson());

  TEST_ASSERT_TRUE(responseJson["topic"] == "precondition");
  TEST_ASSERT_TRUE(responseJson["method"] == "query");
  TEST_ASSERT_TRUE(responseJson["data"]["index"] == 0);
  TEST_ASSERT_FALSE(responseJson["data"]["criteria"].isNull());
}

void q_precondition_RUN_TEST() {
  RUN_TEST(q_precondition_type_is_not_defined);
  RUN_TEST(q_precondition_index_is_not_defined);
  RUN_TEST(q_precondition_index_out_of_range);
  RUN_TEST(q_precondition_no_match_type);
  RUN_TEST(q_precondition_with_timer_param_check_correct_type);
  RUN_TEST(q_precondition_with_criteria_param_check_correct_type);
}