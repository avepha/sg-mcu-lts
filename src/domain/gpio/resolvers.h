#include "validationError.h"
#include "combineContext.h"
#include "domain/resolvers.h"
#include "util/util.h"

#ifndef SG_MCU_GPIO_RESOLVERS_H
#define SG_MCU_GPIO_RESOLVERS_H

class query_gpio_task : public Resolvers {
public:
  explicit query_gpio_task(CombineContext *context) :
      Resolvers("gpio_task", context) {};

  JsonDocument resolve(JsonObject reqData) override {
    std::map<std::string, GpioTask*> gpioTaskMap = context->gpio->core->getGpioTaskMap();

    DynamicJsonDocument data(1024);
    JsonArray gpioTasks = data.createNestedArray("gpioTasks");
    for (std::pair<std::string, GpioTask*> task: gpioTaskMap) {
      JsonObject jo = gpioTasks.createNestedObject();
      jo["channel"] = task.second->getChannel();
      jo["name"] = String(task.second->getTaskName().c_str());
    }

    return data;
  }
};

class query_gpio_state : public Resolvers {
public:
  explicit query_gpio_state(CombineContext *context) :
      Resolvers("gpio_state", context) {};

  JsonDocument resolve(JsonObject reqData) override {
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
