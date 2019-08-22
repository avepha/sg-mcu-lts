#include "../init.h"
#include "../util/nsensor.h"

void q_nsensors_should_have_sensor_1_and_7(){
  uint32_t timestamp = millis();
  publishNSensorToStation(1);
  publishNSensorToStation(7);

  JsonRequest requestTopic("nsensors", "query");
  JsonDocument responseJson = resolvers.execute(requestTopic.toJson());

  TEST_ASSERT_TRUE(responseJson["topic"] == "nsensors");
  TEST_ASSERT_TRUE(responseJson["method"] == "query");

  TEST_ASSERT_TRUE(responseJson["data"][0]["index"] == 1);
  TEST_ASSERT_TRUE(responseJson["data"][0]["isAlive"]);
  TEST_ASSERT_TRUE(responseJson["data"][0]["lastSeen"] >= timestamp);
  TEST_ASSERT_TRUE(responseJson["data"][0]["sensors"].is<JsonObject>());

  TEST_ASSERT_TRUE(responseJson["data"][1]["index"] == 7);
  TEST_ASSERT_TRUE(responseJson["data"][1]["isAlive"]);
  TEST_ASSERT_TRUE(responseJson["data"][1]["lastSeen"] >= timestamp);
  TEST_ASSERT_TRUE(responseJson["data"][1]["sensors"].is<JsonObject>());
}

void q_nsensors_should_have_sensor_1_and_7_withsensor_eq_false(){
  uint32_t timestamp = millis();
  publishNSensorToStation(1);
  publishNSensorToStation(7);
  DynamicJsonDocument data(64);
  data["withSensor"] = false;

  JsonRequest requestTopic("nsensors", "query", data);
  JsonDocument responseJson = resolvers.execute(requestTopic.toJson());

  TEST_ASSERT_TRUE(responseJson["topic"] == "nsensors");
  TEST_ASSERT_TRUE(responseJson["method"] == "query");

  TEST_ASSERT_TRUE(responseJson["data"][0]["index"] == 1);
  TEST_ASSERT_TRUE(responseJson["data"][0]["isAlive"]);
  TEST_ASSERT_TRUE(responseJson["data"][0]["lastSeen"] >= timestamp);
  TEST_ASSERT_TRUE(responseJson["data"][0]["sensors"].is<JsonArray>());

  TEST_ASSERT_TRUE(responseJson["data"][1]["index"] == 7);
  TEST_ASSERT_TRUE(responseJson["data"][1]["isAlive"]);
  TEST_ASSERT_TRUE(responseJson["data"][1]["lastSeen"] >= timestamp);
  TEST_ASSERT_TRUE(responseJson["data"][1]["sensors"].is<JsonArray>());
}


void q_nsensors_RUN_TEST() {
  RUN_TEST(q_nsensors_should_have_sensor_1_and_7);
  RUN_TEST(q_nsensors_should_have_sensor_1_and_7_withsensor_eq_false);
}
