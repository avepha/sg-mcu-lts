#include "../init.h"

void q_sensor() {
  JsonTopic topic("sensor", "query");
  StaticJsonDocument<256> json = topic.toStaticJsonObject();

  String result = resolvers.execute(json.as<JsonObject>());

  StaticJsonDocument<512> resJson;
  DeserializationError error = deserializeJson(resJson, result);
  if (error) {
    TEST_ASSERT_FALSE(error);
  }

  TEST_ASSERT_TRUE(resJson["topic"] == "sensor");
  TEST_ASSERT_TRUE(resJson["method"] == "query");
  TEST_ASSERT_FALSE(resJson["data"].isNull());
}

void q_sensor_RUN_TEST() {
  RUN_TEST(q_sensor);
}