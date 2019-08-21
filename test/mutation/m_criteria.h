#include "../init.h"

void m_criteria_save_with_timer_param_check_correct_type() {
  StaticJsonDocument<256> data;
  data["index"] = 1;
  JsonObject criteria = data.createNestedObject("criteria");
  criteria["sensor"] = 1;
  criteria["criteria"] = 70.1;
  criteria["greater"] = true;
  JsonRequest topic("criteria_save", "mutation", data.as<JsonObject>());
  String topicResult = resolvers.execute(topic.toStaticJsonObject().as<JsonObject>());
  StaticJsonDocument<1024> jsonResult;
  deserializeJson(jsonResult, topicResult);

  TEST_ASSERT_TRUE(jsonResult["topic"] == "criteria_save");
  TEST_ASSERT_TRUE(jsonResult["method"] == "mutation");
  TEST_ASSERT_TRUE(jsonResult["data"]["index"] == 1);
  TEST_ASSERT_TRUE(jsonResult["data"]["writeOps"] > 0);
  TEST_ASSERT_TRUE(jsonResult["data"]["criteria"]["sensor"] == 1);
  TEST_ASSERT_TRUE(jsonResult["data"]["criteria"]["criteria"] == 70.1);
  TEST_ASSERT_TRUE(jsonResult["data"]["criteria"]["greater"]);
}

void m_criteria_save_index_is_not_specified() {
  StaticJsonDocument<256> data;
  JsonObject criteria = data.createNestedObject("criteria");
  criteria["sensor"] = 1;
  criteria["criteria"] = 70.1;
  criteria["greater"] = true;
  JsonRequest topic("criteria_save", "mutation", data.as<JsonObject>());
  String topicResult = resolvers.execute(topic.toStaticJsonObject().as<JsonObject>());
  StaticJsonDocument<1024> jsonResult;
  deserializeJson(jsonResult, topicResult);

  TEST_ASSERT_TRUE(jsonResult["topic"] == "Error");
  TEST_ASSERT_TRUE(jsonResult["code"] == "invalid-input");
  TEST_ASSERT_TRUE(jsonResult["message"] == "index is not specified.");
}

void m_criteria_save_index_out_of_range() {
  StaticJsonDocument<256> data;
  JsonObject criteria = data.createNestedObject("criteria");
  data["index"] = -1;
  criteria["sensor"] = 1;
  criteria["criteria"] = 70.1;
  criteria["greater"] = true;
  JsonRequest topic("criteria_save", "mutation", data.as<JsonObject>());
  String topicResult = resolvers.execute(topic.toStaticJsonObject().as<JsonObject>());
  StaticJsonDocument<1024> jsonResult;
  deserializeJson(jsonResult, topicResult);

  TEST_ASSERT_TRUE(jsonResult["topic"] == "Error");
  TEST_ASSERT_TRUE(jsonResult["code"] == "invalid-input");
  TEST_ASSERT_TRUE(jsonResult["message"] == "index out of range.");
}

void m_criteria_save_RUN_TEST() {
  RUN_TEST(m_criteria_save_with_timer_param_check_correct_type);
  RUN_TEST(m_criteria_save_index_is_not_specified);
  RUN_TEST(m_criteria_save_index_out_of_range);
}
