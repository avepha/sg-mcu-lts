#include "../init.h"

class t_channel {
public:
  t_channel() {
    RUN_TEST(channel_should_be_of_correct_value);
  }

  static void channel_should_be_of_correct_value() {
    DynamicJsonDocument data_1(256);
    data_1["index"] = 0;
    JsonObject control = data_1.createNestedObject("control");
    control["type"] = "timer";
    control["value"] = 1;

    JsonArray preconditions = data_1.createNestedArray("preconditions");
    JsonObject precondition_1 = preconditions.createNestedObject();
    precondition_1["type"] = "timer";
    precondition_1["value"] = 1;

    JsonObject precondition_2 = preconditions.createNestedObject();
    precondition_2["type"] = "criteria";
    precondition_2["value"] = 1;

    JsonRequest requestTopic_1("channel_save", "mutation", data_1);
    resolvers->execute(requestTopic_1.toJson());

    DynamicJsonDocument data(256);
    data["index"] = 0;
    JsonRequest requestTopic("channel", "query", data);
    JsonDocument responseJson = resolvers->execute(requestTopic.toJson());

    TEST_ASSERT_TRUE(responseJson["topic"] == "channel");
    TEST_ASSERT_TRUE(responseJson["method"] == "query");
    TEST_ASSERT_TRUE(responseJson["data"]["index"] == 0);
    TEST_ASSERT_TRUE(responseJson["data"]["control"]["type"] == "timer");
    TEST_ASSERT_TRUE(responseJson["data"]["control"]["value"] == 1);
    TEST_ASSERT_TRUE(responseJson["data"]["preconditions"].size() == 2);
    TEST_ASSERT_TRUE(responseJson["data"]["preconditions"][0]["type"] == "timer");
    TEST_ASSERT_TRUE(responseJson["data"]["preconditions"][0]["value"] == 1);
    TEST_ASSERT_TRUE(responseJson["data"]["preconditions"][1]["type"] == "criteria");
    TEST_ASSERT_TRUE(responseJson["data"]["preconditions"][1]["value"] == 1);
  }
};
