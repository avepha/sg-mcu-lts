#include "validationError.h"
#include "combineContext.h"

#include "util/util.h"

#ifndef SG_MCU_GPIO_RESOLVERS_H
#define SG_MCU_GPIO_RESOLVERS_H

class query_gpio_task : public Query {
public:
  explicit query_gpio_task() : Query("gpio_task") {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    DynamicJsonDocument data(1024);
    JsonArray gpioTasks = data.createNestedArray("gpioTasks");

    for (uint8_t i = 0; i < 8; i++) {
      auto *control = context->channel->core->getControlByChannel(i);
      if (control != nullptr) {
        JsonObject jo = gpioTasks.createNestedObject();
        jo["channel"] = i;
        jo["uid"] = ChannelControlEnumToShortString(control->getType());
        jo["type"] = "no_expired";
      }
    }

    return data;
  }
};

class query_gpio_state : public Query {
public:
  explicit query_gpio_state() : Query("gpio_state") {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    uint8_t *state = context->gpio->gpioState->getStates();

    DynamicJsonDocument data(1024);
    for (int i = 0; i < 8; i++) {
      JsonObject channel = data.createNestedObject();
      channel["channel"] = i;
      channel["state"] = state[i] == HIGH ? "HIGH" : "LOW";
    }

    return data;
  }
};

#endif //SG_MCU_RESOLVERS_H
