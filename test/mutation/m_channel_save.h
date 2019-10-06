#include "../init.h"

class t_channel_save {
public:
  t_channel_save() {
    RUN_TEST(channel_save_should_be_of_correct_value);
  }

private:
  static void channel_save_should_be_of_correct_value() {
    DynamicJsonDocument data(256);
    data["index"] = 0;
    JsonObject control = data.createNestedObject("control");
    control["type"] = "timer";
    control["value"] = 1;

    JsonArray preconditions = data.createNestedArray("preconditions");
    JsonObject precondition_1 = preconditions.createNestedObject();
    precondition_1["type"] = "timer";
    precondition_1["value"] = 1;

    JsonObject precondition_2 = preconditions.createNestedObject();
    precondition_2["type"] = "criteria";
    precondition_2["value"] = 1;

    JsonRequest requestTopic("channel_save", "mutation", data);
    JsonDocument responseJson = resolvers->execute(requestTopic.toJson());

    TEST_ASSERT_TRUE(responseJson["topic"] == "channel_save");
    TEST_ASSERT_TRUE(responseJson["method"] == "mutation");
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