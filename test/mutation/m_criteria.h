#include "../init.h"

void m_criteria_save_with_timer_param_check_correct_type() {
  DynamicJsonDocument data(256);
  data["index"] = 1;
  JsonObject criteria = data.createNestedObject("criteria");
  criteria["sensor"] = 1;
  criteria["criteria"] = 70.1;
  criteria["greater"] = true;
  JsonRequest requestTopic("criteria_save", "mutation", data);
  JsonDocument responseJson = resolvers->execute(requestTopic.toJson());

  TEST_ASSERT_TRUE(responseJson["topic"] == "criteria_save");
  TEST_ASSERT_TRUE(responseJson["method"] == "mutation");
  TEST_ASSERT_TRUE(responseJson["data"]["index"] == 1);
  TEST_ASSERT_TRUE(responseJson["data"]["writeOps"] > 0);
  TEST_ASSERT_TRUE(responseJson["data"]["criteria"]["sensor"] == 1);
  TEST_ASSERT_TRUE(responseJson["data"]["criteria"]["criteria"] == 70.1);
  TEST_ASSERT_TRUE(responseJson["data"]["criteria"]["greater"]);
}

void m_criteria_save_index_is_not_specified() {
  DynamicJsonDocument data(256);
  JsonObject criteria = data.createNestedObject("criteria");
  criteria["sensor"] = 1;
  criteria["criteria"] = 70.1;
  criteria["greater"] = true;
  JsonRequest requestTopic("criteria_save", "mutation", data);
  JsonDocument responseJson = resolvers->execute(requestTopic.toJson());

  TEST_ASSERT_TRUE(responseJson["topic"] == "Error");
  TEST_ASSERT_TRUE(responseJson["code"] == "invalid-input");
  TEST_ASSERT_TRUE(responseJson["message"] == "index is not specified.");
}

void m_criteria_save_index_out_of_range() {
  DynamicJsonDocument data(256);
  JsonObject criteria = data.createNestedObject("criteria");
  data["index"] = -1;
  criteria["sensor"] = 1;
  criteria["criteria"] = 70.1;
  criteria["greater"] = true;
  JsonRequest requestTopic("criteria_save", "mutation", data);
  JsonDocument responseJson = resolvers->execute(requestTopic.toJson());

  TEST_ASSERT_TRUE(responseJson["topic"] == "Error");
  TEST_ASSERT_TRUE(responseJson["code"] == "invalid-input");
  TEST_ASSERT_TRUE(responseJson["message"] == "index out of range.");
}

void m_criteria_save_RUN_TEST() {
  RUN_TEST(m_criteria_save_with_timer_param_check_correct_type);
  RUN_TEST(m_criteria_save_index_is_not_specified);
  RUN_TEST(m_criteria_save_index_out_of_range);
}
