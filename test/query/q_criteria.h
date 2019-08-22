#include "../init.h"

void q_criteria_with_timer_param_check_correct_type() {
  StaticJsonDocument<256> data;
  data["index"] = 1;
  JsonRequest topic("criteria", "query", data);
  String topicResult = resolvers.execute(topic.toStaticJsonObject().as<JsonObject>());
  StaticJsonDocument<1024> jsonResult;
  deserializeJson(jsonResult, topicResult);

  TEST_ASSERT_TRUE(jsonResult["topic"] == "criteria");
  TEST_ASSERT_TRUE(jsonResult["method"] == "query");
  TEST_ASSERT_TRUE(jsonResult["data"]["index"] == 1);
  TEST_ASSERT_FALSE(jsonResult["data"]["criteria"]["sensor"].isNull());
  TEST_ASSERT_FALSE(jsonResult["data"]["criteria"]["criteria"].isNull());
  TEST_ASSERT_FALSE(jsonResult["data"]["criteria"]["greater"].isNull());
}

void q_criteria_index_is_not_specified() {
  StaticJsonDocument<256> data;
  JsonRequest topic("criteria", "query", data);
  String topicResult = resolvers.execute(topic.toStaticJsonObject().as<JsonObject>());
  StaticJsonDocument<1024> jsonResult;
  deserializeJson(jsonResult, topicResult);

  TEST_ASSERT_TRUE(jsonResult["topic"] == "Error");
  TEST_ASSERT_TRUE(jsonResult["code"] == "invalid-input");
  TEST_ASSERT_TRUE(jsonResult["message"] == "index is not specified.");
}

void q_criteria_index_out_of_range() {
  StaticJsonDocument<256> data;
  data["index"] = -1;
  JsonRequest topic("criteria", "query", data);
  String topicResult = resolvers.execute(topic.toStaticJsonObject().as<JsonObject>());
  StaticJsonDocument<1024> jsonResult;
  deserializeJson(jsonResult, topicResult);

  TEST_ASSERT_TRUE(jsonResult["topic"] == "Error");
  TEST_ASSERT_TRUE(jsonResult["code"] == "invalid-input");
  TEST_ASSERT_TRUE(jsonResult["message"] == "index out of range.");
}

void q_criteria_RUN_TEST() {
  RUN_TEST(q_criteria_with_timer_param_check_correct_type);
  RUN_TEST(q_criteria_index_is_not_specified);
  RUN_TEST(q_criteria_index_out_of_range);
}
