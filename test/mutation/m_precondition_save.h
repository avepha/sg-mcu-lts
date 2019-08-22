#include "../init.h"

void m_precondition_save_with_timer_param_check_correct_type() {
  StaticJsonDocument<256> data;
  data["index"] = 0;
  JsonArray timers = data.createNestedArray("timers");
  JsonArray timer_0 = timers.createNestedArray();
  timer_0.add(0);
  timer_0.add(3600);

  JsonArray timer_1 = timers.createNestedArray();
  timer_1.add(7200);
  timer_1.add(10800);

  JsonRequest topic("precondition_save", "mutation", data);
  String topicResult = resolvers.execute(topic.toStaticJsonObject().as<JsonObject>());
  StaticJsonDocument<1024> jsonResult;
  deserializeJson(jsonResult, topicResult);

  TEST_ASSERT_TRUE(jsonResult["topic"] == "precondition_save");
  TEST_ASSERT_TRUE(jsonResult["method"] == "mutation");
  TEST_ASSERT_TRUE(jsonResult["data"]["timers"][0][0].as<int>() == 0);
  TEST_ASSERT_TRUE(jsonResult["data"]["timers"][0][1].as<int>() == 3600);
  TEST_ASSERT_TRUE(jsonResult["data"]["timers"][1][0].as<int>() == 7200);
  TEST_ASSERT_TRUE(jsonResult["data"]["timers"][1][1].as<int>() == 10800);
  TEST_ASSERT_TRUE(jsonResult["data"]["index"] == 0);
  TEST_ASSERT_GREATER_OR_EQUAL(1, jsonResult["data"]["writeOps"]);
}

void m_precondition_save_with_criteria_param_check_correct_type() {
  StaticJsonDocument<256> data;
  data["index"] = 0;
  JsonObject criteria = data.createNestedObject("criteria");
  criteria["sensor"] = 1;
  criteria["criteria"] = 99.0;
  criteria["greater"] = false;

  JsonRequest topic("precondition_save", "mutation", data);
  String topicResult = resolvers.execute(topic.toStaticJsonObject().as<JsonObject>());
  StaticJsonDocument<1024> jsonResult;
  deserializeJson(jsonResult, topicResult);

  TEST_ASSERT_TRUE(jsonResult["topic"] == "precondition_save");
  TEST_ASSERT_TRUE(jsonResult["method"] == "mutation");
  TEST_ASSERT_TRUE(jsonResult["data"]["index"] == 0);
  TEST_ASSERT_GREATER_OR_EQUAL(1, jsonResult["data"]["writeOps"]);
  TEST_ASSERT_FALSE(jsonResult["data"]["criteria"].isNull());
  TEST_ASSERT_TRUE(jsonResult["data"]["criteria"]["sensor"] == 1);
  TEST_ASSERT_TRUE(jsonResult["data"]["criteria"]["criteria"] == 99.0);
  TEST_ASSERT_FALSE(jsonResult["data"]["criteria"]["greater"]);
}

void m_precondition_save_index_is_not_defined() {
  StaticJsonDocument<256> data;
  JsonArray timers = data.createNestedArray("timers");
  JsonArray timer_0 = timers.createNestedArray();
  timer_0.add(0);
  timer_0.add(3600);

  JsonArray timer_1 = timers.createNestedArray();
  timer_1.add(7200);
  timer_1.add(10800);

  JsonRequest topic("precondition_save", "mutation", data);
  String topicResult = resolvers.execute(topic.toStaticJsonObject().as<JsonObject>());
  StaticJsonDocument<1024> jsonResult;
  deserializeJson(jsonResult, topicResult);

  TEST_ASSERT_TRUE(jsonResult["topic"] == "Error");
  TEST_ASSERT_TRUE(jsonResult["code"] == "invalid-input");
  TEST_ASSERT_TRUE(jsonResult["message"] == "index or timers field is not specified.");
}

void m_precondition_save_index_out_of_range() {
  DynamicJsonDocument data(64);
  data["type"] = "timer";
  data["index"] = -1;
  JsonRequest topic("precondition", "query", data);
  StaticJsonDocument<256> json = topic.toStaticJsonObject();

  String result = resolvers.execute(json.as<JsonObject>());

  StaticJsonDocument<512> resJson;
  deserializeJson(resJson, result);

  TEST_ASSERT_TRUE(resJson["topic"] == "Error");
  TEST_ASSERT_TRUE(resJson["code"] == "invalid-input");
  TEST_ASSERT_TRUE(resJson["message"] == "index out of range.");
}

void m_precondition_save_RUN_TEST() {
  RUN_TEST(m_precondition_save_with_timer_param_check_correct_type);
  RUN_TEST(m_precondition_save_with_criteria_param_check_correct_type);
  RUN_TEST(m_precondition_save_index_is_not_defined);
  RUN_TEST(m_precondition_save_index_out_of_range);
}
