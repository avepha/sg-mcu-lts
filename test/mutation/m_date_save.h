#include "../init.h"

void m_date_save_check_correct_type() {
  DynamicJsonDocument data(64);
  data["date"] = "2019-08-10T05:01:02";
  JsonRequest m_date_save("date_save", "mutation", data);
  String m_date_save_res = resolvers.execute(m_date_save.toStaticJsonObject().as<JsonObject>());

  StaticJsonDocument<256> m_date_save_json;
  DeserializationError m_date_save_error = deserializeJson(m_date_save_json, m_date_save_res);
  if (m_date_save_error) {
    TEST_ASSERT_FALSE(m_date_save_error);
  }
  
  TEST_ASSERT_TRUE(m_date_save_json["topic"] == "date_save");
  TEST_ASSERT_TRUE(m_date_save_json["method"] == "mutation");
  TEST_ASSERT_FALSE(m_date_save_json["data"].isNull());
  TEST_ASSERT_TRUE(m_date_save_json["data"]["date"].as<String>().startsWith( "2019-08-10T05:01:02"));
}

void m_date_save_throw_date_is_not_define() {
  DynamicJsonDocument data(64);
  JsonRequest m_date_save("date_save", "mutation", data);
  String m_date_save_res = resolvers.execute(m_date_save.toStaticJsonObject().as<JsonObject>());

  StaticJsonDocument<256> m_date_save_json;
  DeserializationError m_date_save_errpr = deserializeJson(m_date_save_json, m_date_save_res);
  if (m_date_save_errpr) {
    TEST_ASSERT_FALSE(m_date_save_errpr);
  }

  TEST_ASSERT_TRUE(m_date_save_json["topic"] == "Error");
  TEST_ASSERT_TRUE(m_date_save_json["code"] == "invalid-input");
}

void m_date_save_RUN_TEST() {
  RUN_TEST(m_date_save_check_correct_type);
  RUN_TEST(m_date_save_throw_date_is_not_define);
}
