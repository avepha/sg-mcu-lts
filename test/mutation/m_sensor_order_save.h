#include "../init.h"

void m_sensor_order_save_check_correct_type() {
  StaticJsonDocument<256> data;
  JsonArray names = data.createNestedArray("names");
  names.add("sensor_1");
  names.add("sensor_2");

  names.add("sensor_1");
  names.add("sensor_2");

  JsonRequest m_date_save("sensor_order_save", "mutation", data);
  String m_sensor_order_save_res = resolvers.execute(m_date_save.toStaticJsonObject().as<JsonObject>());

  StaticJsonDocument<512> m_sensor_order_save;
  DeserializationError m_sensor_order_save_err = deserializeJson(m_sensor_order_save, m_sensor_order_save_res);
  if (m_sensor_order_save_err) {
    TEST_ASSERT_FALSE(m_sensor_order_save_err);
  }

  TEST_ASSERT_TRUE(m_sensor_order_save["topic"] == "sensor_order_save");
  TEST_ASSERT_TRUE(m_sensor_order_save["method"] == "mutation");
  TEST_ASSERT_FALSE(m_sensor_order_save["data"]["names"].isNull());
  TEST_ASSERT_TRUE(m_sensor_order_save["data"]["names"][0] == "sensor_1");
  TEST_ASSERT_TRUE(m_sensor_order_save["data"]["names"][1] == "sensor_2");
  TEST_ASSERT_GREATER_OR_EQUAL(1, m_sensor_order_save["data"]["writeOps"]);
}

void m_sensor_order_save_names_is_not_define() {
  DynamicJsonDocument data(64);
  JsonRequest m_sensor_order_save("sensor_order_save", "mutation", data);
  String m_sensor_order_save_res = resolvers.execute(m_sensor_order_save.toStaticJsonObject().as<JsonObject>());

  StaticJsonDocument<256> m_sensor_order_save_json;
  DeserializationError m_sensor_order_save_error = deserializeJson(m_sensor_order_save_json, m_sensor_order_save_res);
  if (m_sensor_order_save_error) {
    TEST_ASSERT_FALSE(m_sensor_order_save_error);
  }

  TEST_ASSERT_TRUE(m_sensor_order_save_json["topic"] == "Error");
  TEST_ASSERT_TRUE(m_sensor_order_save_json["code"] == "invalid-input");
}

void m_sensor_order_save_RUN_TEST() {
  RUN_TEST(m_sensor_order_save_check_correct_type);
  RUN_TEST(m_sensor_order_save_names_is_not_define);
}
