#include "../init.h"

void q_timer_index_is_not_defined() {
  DynamicJsonDocument data(64);
  JsonRequest topic("timer", "query", data);
  StaticJsonDocument<256> json = topic.toStaticJsonObject();
  String result = resolvers.execute(json.as<JsonObject>());

  StaticJsonDocument<512> resJson;
  DeserializationError error = deserializeJson(resJson, result);
  if (error) {
    TEST_ASSERT_FALSE(error);
  }

  TEST_ASSERT_TRUE(resJson["topic"] == "Error");
  TEST_ASSERT_TRUE(resJson["code"] == "invalid-input");
  TEST_ASSERT_TRUE(resJson["message"] == "index is not specified.");
}

void q_timer_index_out_of_range() {
  DynamicJsonDocument data(64);
  data["index"] = -1;
  JsonRequest topic("timer", "query", data);
  StaticJsonDocument<256> json = topic.toStaticJsonObject();
  String result = resolvers.execute(json.as<JsonObject>());

  StaticJsonDocument<512> resJson;
  deserializeJson(resJson, result);

  TEST_ASSERT_TRUE(resJson["topic"] == "Error");
  TEST_ASSERT_TRUE(resJson["code"] == "invalid-input");
  TEST_ASSERT_TRUE(resJson["message"] == "index out of range.");
}

void q_timer_check_correct_type() {
  DynamicJsonDocument data(64);
  data["type"] = "timer";
  data["index"] = 0;
  JsonRequest topic("timer", "query", data);
  String result = resolvers.execute(topic.toStaticJsonObject().as<JsonObject>());

  StaticJsonDocument<512> resJson;
  DeserializationError error = deserializeJson(resJson, result);
  if (error) {
    TEST_ASSERT_FALSE(error);
  }

  TEST_ASSERT_TRUE(resJson["topic"] == "timer");
  TEST_ASSERT_TRUE(resJson["method"] == "query");
  TEST_ASSERT_TRUE(resJson["data"]["index"] == 0);
  TEST_ASSERT_TRUE(!resJson["data"]["timers"].isNull());
}

void q_timer_RUN_TEST() {
  RUN_TEST(q_timer_index_is_not_defined);
  RUN_TEST(q_timer_index_out_of_range);
  RUN_TEST(q_timer_check_correct_type);
}