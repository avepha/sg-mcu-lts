#include "../init.h"

void q_date_check_correct_type() {
  JsonRequest topic("date", "query");
  StaticJsonDocument<256> json = topic.toStaticJsonObject();

  String result = resolvers.execute(json.as<JsonObject>());

  StaticJsonDocument<256> resJson;
  DeserializationError error = deserializeJson(resJson, result);
  if (error) {
    TEST_ASSERT_FALSE(error);
  }
  TEST_ASSERT_TRUE(resJson["topic"] == "date");
  TEST_ASSERT_TRUE(resJson["method"] == "query");
  TEST_ASSERT_TRUE(resJson["data"].as<String>().endsWith("000Z"));
}

void q_date_RUN_TEST() {
  RUN_TEST(q_date_check_correct_type);
}