#include "../init.h"

void meta_field_should_return_null_when_query_without_reqId(){
  JsonRequest topic("date", "query");
  String topicResult = resolvers.execute(topic.toStaticJsonObject().as<JsonObject>());

  StaticJsonDocument<1024> jsonResult;
  deserializeJson(jsonResult, topicResult);

  TEST_ASSERT_TRUE(jsonResult["topic"] == "date");
  TEST_ASSERT_TRUE(jsonResult["method"] == "query");
  TEST_ASSERT_TRUE(jsonResult["reqId"].isNull());
}

void meta_field_should_return_reqId_when_query_with_reqId_eq_req111(){
  JsonRequest topic("date", "query", "req111");
  String topicResult = resolvers.execute(topic.toStaticJsonObject().as<JsonObject>());

  StaticJsonDocument<1024> jsonResult;
  deserializeJson(jsonResult, topicResult);
  serializeJson(jsonResult, Serial);

  TEST_ASSERT_TRUE(jsonResult["topic"] == "date");
  TEST_ASSERT_TRUE(jsonResult["method"] == "query");
  TEST_ASSERT_FALSE(jsonResult["reqId"].isNull());
  TEST_ASSERT_TRUE(jsonResult["reqId"] == "req111");
}


void meta_field_RUN_TEST() {
  RUN_TEST(meta_field_should_return_null_when_query_without_reqId);
  RUN_TEST(meta_field_should_return_reqId_when_query_with_reqId_eq_req111);
}
