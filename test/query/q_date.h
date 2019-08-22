#include "../init.h"

void q_date_check_correct_type() {
  JsonRequest requestTopic("date", "query");

  JsonDocument responseJson = resolvers.execute(requestTopic.toJson());

  TEST_ASSERT_TRUE(responseJson["topic"] == "date");
  TEST_ASSERT_TRUE(responseJson["method"] == "query");
  TEST_ASSERT_FALSE(responseJson["data"].isNull());
  TEST_ASSERT_TRUE(responseJson["data"]["date"].as<String>().endsWith("000Z"));
}

void q_date_RUN_TEST() {
  RUN_TEST(q_date_check_correct_type);
}