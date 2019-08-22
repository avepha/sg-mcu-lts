#include "../init.h"

void m_timer_save_check_correct_type() {
  DynamicJsonDocument data(256);
  data["index"] = 0;
  JsonArray timers = data.createNestedArray("timers");
  JsonArray timer_0 = timers.createNestedArray();
  timer_0.add(0);
  timer_0.add(3600);

  JsonArray timer_1 = timers.createNestedArray();
  timer_1.add(7200);
  timer_1.add(10800);

  JsonRequest requestTopic("timer_save", "mutation", data);
  JsonDocument responseJson = resolvers.execute(requestTopic.toJson());

  TEST_ASSERT_TRUE(responseJson["topic"] == "timer_save");
  TEST_ASSERT_TRUE(responseJson["method"] == "mutation");
  TEST_ASSERT_TRUE(responseJson["data"]["timers"][0][0].as<int>() == 0);
  TEST_ASSERT_TRUE(responseJson["data"]["timers"][0][1].as<int>() == 3600);
  TEST_ASSERT_TRUE(responseJson["data"]["timers"][1][0].as<int>() == 7200);
  TEST_ASSERT_TRUE(responseJson["data"]["timers"][1][1].as<int>() == 10800);
  TEST_ASSERT_TRUE(responseJson["data"]["index"] == 0);
  TEST_ASSERT_GREATER_OR_EQUAL(1, responseJson["data"]["writeOps"]);
}

void m_timer_save_index_is_not_defined() {
  DynamicJsonDocument data(256);
  JsonArray timers = data.createNestedArray("timers");
  JsonArray timer_0 = timers.createNestedArray();
  timer_0.add(0);
  timer_0.add(3600);

  JsonArray timer_1 = timers.createNestedArray();
  timer_1.add(7200);
  timer_1.add(10800);

  JsonRequest requestTopic("timer_save", "mutation", data);

  JsonDocument responseJson = resolvers.execute(requestTopic.toJson());

  TEST_ASSERT_TRUE(responseJson["topic"] == "Error");
  TEST_ASSERT_TRUE(responseJson["code"] == "invalid-input");
  TEST_ASSERT_TRUE(responseJson["message"] == "index or timers field is not specified.");
}

void m_timer_save_index_out_of_range() {
  DynamicJsonDocument data(256);
  JsonArray timers = data.createNestedArray("timers");
  JsonArray timer_0 = timers.createNestedArray();
  timer_0.add(0);
  timer_0.add(3600);

  data["index"] = -1;
  JsonRequest requestTopic("timer_save", "mutation", data);
  JsonDocument responseJson = resolvers.execute(requestTopic.toJson());

  TEST_ASSERT_TRUE(responseJson["topic"] == "Error");
  TEST_ASSERT_TRUE(responseJson["code"] == "invalid-input");
  TEST_ASSERT_TRUE(responseJson["message"] == "index out of range.");
}

void m_timer_save_RUN_TEST() {
  RUN_TEST(m_timer_save_check_correct_type);
  RUN_TEST(m_timer_save_index_is_not_defined);
  RUN_TEST(m_timer_save_index_out_of_range);
}
