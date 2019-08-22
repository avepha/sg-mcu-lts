#include "../init.h"

void q_precondition_type_is_not_defined() {
  DynamicJsonDocument data(64);
  data["index"] = 0;
  JsonRequest topic("precondition", "query", data);
  String result = resolvers.execute(topic.toStaticJsonObject().as<JsonObject>());

  StaticJsonDocument<512> resJson;
  DeserializationError error = deserializeJson(resJson, result);
  if (error) {
    TEST_ASSERT_FALSE(error);
  }

  TEST_ASSERT_TRUE(resJson["topic"] == "Error");
  TEST_ASSERT_TRUE(resJson["code"] == "invalid-input");
  TEST_ASSERT_TRUE(resJson["message"] == "type Or index field is not specified.");
}

void q_precondition_index_is_not_defined() {
  DynamicJsonDocument data(64);
  data["type"] = "timer";
  JsonRequest topic("precondition", "query", data);
  StaticJsonDocument<256> json = topic.toStaticJsonObject();
  String result = resolvers.execute(json.as<JsonObject>());

  StaticJsonDocument<512> resJson;
  DeserializationError error = deserializeJson(resJson, result);
  if (error) {
    TEST_ASSERT_FALSE(error);
  }

  TEST_ASSERT_TRUE(resJson["topic"] == "Error");
  TEST_ASSERT_TRUE(resJson["code"] == "invalid-input");
  TEST_ASSERT_TRUE(resJson["message"] == "type Or index field is not specified.");
}

void q_precondition_index_out_of_range() {
  DynamicJsonDocument data(64);
  data["type"] = "timer";
  data["index"] = -1;
  JsonRequest topic("precondition", "query", data);
  StaticJsonDocument<256> json = topic.toStaticJsonObject();

  String result = resolvers.execute(json.as<JsonObject>());

  StaticJsonDocument<512> resJson;
  DeserializationError error = deserializeJson(resJson, result);
  if (error) {
    TEST_ASSERT_FALSE(error);
  }

  TEST_ASSERT_TRUE(resJson["topic"] == "Error");
  TEST_ASSERT_TRUE(resJson["code"] == "invalid-input");
  TEST_ASSERT_TRUE(resJson["message"] == "index out of range.");
}

void q_precondition_no_match_type() {
  DynamicJsonDocument data(64);
  data["type"] = "nomatch";
  data["index"] = 0;
  JsonRequest topic("precondition", "query", data);
  String result = resolvers.execute(topic.toStaticJsonObject().as<JsonObject>());

  StaticJsonDocument<512> resJson;
  DeserializationError error = deserializeJson(resJson, result);
  if (error) {
    TEST_ASSERT_FALSE(error);
  }

  TEST_ASSERT_TRUE(resJson["topic"] == "Error");
  TEST_ASSERT_TRUE(resJson["code"] == "invalid-input");
  TEST_ASSERT_TRUE(resJson["message"] == "No such the provided type.");
}

void q_precondition_with_timer_param_check_correct_type() {
  DynamicJsonDocument data(64);
  data["type"] = "timer";
  data["index"] = 0;
  JsonRequest topic("precondition", "query", data);
  String result = resolvers.execute(topic.toStaticJsonObject().as<JsonObject>());

  StaticJsonDocument<512> resJson;
  DeserializationError error = deserializeJson(resJson, result);
  if (error) {
    TEST_ASSERT_FALSE(error);
  }

  TEST_ASSERT_TRUE(resJson["topic"] == "precondition");
  TEST_ASSERT_TRUE(resJson["method"] == "query");
  TEST_ASSERT_TRUE(resJson["data"]["index"] == 0);
  TEST_ASSERT_TRUE(!resJson["data"]["timers"].isNull());
}

void q_precondition_with_criteria_param_check_correct_type() {
  DynamicJsonDocument data(64);
  data["type"] = "criteria";
  data["index"] = 0;
  JsonRequest topic("precondition", "query", data);
  String result = resolvers.execute(topic.toStaticJsonObject().as<JsonObject>());

  StaticJsonDocument<512> resJson;
  DeserializationError error = deserializeJson(resJson, result);
  if (error) {
    TEST_ASSERT_FALSE(error);
  }

  TEST_ASSERT_TRUE(resJson["topic"] == "precondition");
  TEST_ASSERT_TRUE(resJson["method"] == "query");
  TEST_ASSERT_TRUE(resJson["data"]["index"] == 0);
  TEST_ASSERT_FALSE(resJson["data"]["criteria"].isNull());
}

void q_precondition_RUN_TEST() {
  RUN_TEST(q_precondition_type_is_not_defined);
  RUN_TEST(q_precondition_index_is_not_defined);
  RUN_TEST(q_precondition_index_out_of_range);
  RUN_TEST(q_precondition_no_match_type);
  RUN_TEST(q_precondition_with_timer_param_check_correct_type);
  RUN_TEST(q_precondition_with_criteria_param_check_correct_type);
}