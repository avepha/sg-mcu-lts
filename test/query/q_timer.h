#include "../init.h"

void q_timer_index_is_not_defined() {
  StaticJsonDocument<64> data;
  JsonTopic topic("timer", "query", data.as<JsonObject>());
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
  StaticJsonDocument<64> data;
  data["index"] = -1;
  JsonTopic topic("timer", "query", data.as<JsonObject>());
  StaticJsonDocument<256> json = topic.toStaticJsonObject();
  String result = resolvers.execute(json.as<JsonObject>());

  StaticJsonDocument<512> resJson;
  deserializeJson(resJson, result);

  TEST_ASSERT_TRUE(resJson["topic"] == "Error");
  TEST_ASSERT_TRUE(resJson["code"] == "invalid-input");
  TEST_ASSERT_TRUE(resJson["message"] == "index out of range.");
}

void q_timer_check_correct_type() {
  StaticJsonDocument<64> data;
  data["type"] = "timer";
  data["index"] = 0;
  JsonTopic topic("timer", "query", data.as<JsonObject>());
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