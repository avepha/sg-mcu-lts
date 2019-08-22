#include "../init.h"

void m_date_save_check_correct_type() {
  DynamicJsonDocument data(64);
  data["date"] = "2019-08-10T05:01:02";
  JsonRequest requestTopic("date_save", "mutation", data);
  JsonDocument responseJson = resolvers.execute(requestTopic.toJson());

  TEST_ASSERT_TRUE(responseJson["topic"] == "date_save");
  TEST_ASSERT_TRUE(responseJson["method"] == "mutation");
  TEST_ASSERT_FALSE(responseJson["data"].isNull());
  TEST_ASSERT_TRUE(responseJson["data"]["date"].as<String>().startsWith( "2019-08-10T05:01:02"));
}

void m_date_save_throw_date_is_not_define() {
  DynamicJsonDocument data(64);
  JsonRequest requestTopic("date_save", "mutation", data);
  JsonDocument responseJson = resolvers.execute(requestTopic.toJson());

  TEST_ASSERT_TRUE(responseJson["topic"] == "Error");
  TEST_ASSERT_TRUE(responseJson["code"] == "invalid-input");
}

void m_date_save_RUN_TEST() {
  RUN_TEST(m_date_save_check_correct_type);
  RUN_TEST(m_date_save_throw_date_is_not_define);
}
