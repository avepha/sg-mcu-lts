#include "../init.h"

void m_sensor_order_save_check_correct_type() {
  DynamicJsonDocument data(256);
  JsonArray names = data.createNestedArray("names");
  names.add("sensor_1");
  names.add("sensor_2");

  names.add("sensor_1");
  names.add("sensor_2");

  JsonRequest requestTopic("sensor_order_save", "mutation", data);
  JsonDocument responseJson = resolvers.execute(requestTopic.toJson());

  TEST_ASSERT_TRUE(responseJson["topic"] == "sensor_order_save");
  TEST_ASSERT_TRUE(responseJson["method"] == "mutation");
  TEST_ASSERT_FALSE(responseJson["data"]["names"].isNull());
  TEST_ASSERT_TRUE(responseJson["data"]["names"][0] == "sensor_1");
  TEST_ASSERT_TRUE(responseJson["data"]["names"][1] == "sensor_2");
  TEST_ASSERT_GREATER_OR_EQUAL(1, responseJson["data"]["writeOps"]);
}

void m_sensor_order_save_names_is_not_define() {
  DynamicJsonDocument data(64);
  JsonRequest requestTopic("sensor_order_save", "mutation", data);
  JsonDocument responseJson = resolvers.execute(requestTopic.toJson());

  TEST_ASSERT_TRUE(responseJson["topic"] == "Error");
  TEST_ASSERT_TRUE(responseJson["code"] == "invalid-input");
}

void m_sensor_order_save_RUN_TEST() {
  RUN_TEST(m_sensor_order_save_check_correct_type);
  RUN_TEST(m_sensor_order_save_names_is_not_define);
}
