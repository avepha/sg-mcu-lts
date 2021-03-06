#include "validationError.h"
#include "combineContext.h"

#include "util/util.h"

#ifndef SG_MCU_GPIO_RESOLVERS_H
#define SG_MCU_GPIO_RESOLVERS_H

class query_gpio_task : public Query {
public:
  explicit query_gpio_task() : Query("gpio_task") {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    std::map<std::string, GpioTask*> gpioTaskMap = context->gpio->core->getGpioTaskMap();

    DynamicJsonDocument data(1024);
    JsonArray gpioTasks = data.createNestedArray("gpioTasks");
    for (std::pair<std::string, GpioTask*> task: gpioTaskMap) {
      JsonObject jo = gpioTasks.createNestedObject();
      jo["channel"] = task.second->getChannel();
      jo["uid"] = String(task.second->getUid().c_str());
      jo["type"] = task.second->getType() == GPIO_TASK_EXPIRED ? "expired" : "no_expired";
      if (task.second->getType() == GPIO_TASK_EXPIRED) {
        jo["timeout"] = task.second->getTimeout();
        jo["until"] = task.second->untilTimeout();
      }
    }

    return data;
  }
};

class query_gpio_state : public Query {
public:
  explicit query_gpio_state() : Query("gpio_state") {};

  DynamicJsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    std::array<uint8_t , 8> channelState = context->gpio->core->getGpioState();

    DynamicJsonDocument data(1024);
    for (int i = 0; i < channelState.size(); i++) {
      JsonObject channel = data.createNestedObject();
      channel["channel"] = i;
      channel["state"] = channelState[i] == HIGH ? "HIGH" : "LOW";
    }

    return data;
  }
};

#endif //SG_MCU_RESOLVERS_H
