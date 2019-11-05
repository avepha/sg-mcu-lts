#include "combineContext.h"
#include "validationError.h"

#include "domain/mutation.h"
#include "domain/query.h"

#include "domain/rtc/resolvers.h"
#include "domain/nvmemory/resolvers.h"
#include "domain/info/resolvers.h"

#include "domain/rtc/resolvers.h"
#include "domain/sensor/resolvers.h"
#include "domain/nsensor/resolvers.h"
#include "domain/gpio/resolvers.h"

#include "domain/control/resolvers.h"

#include "domain/channel/resolvers.h"
#include "domain/channel-control/criteria/resolvers.h"
#include "domain/channel-control/timer/resolvers.h"
#include "domain/channel-control/range/resolvers.h"

#include "domain/continuous/resolvers.h"
#include "domain/continuous-control/continuous-criteria/resolvers.h"

#include "domain/precondition/criteria/resolvers.h"
#include "domain/precondition/timer/resolvers.h"
#include "domain/precondition/range/resolvers.h"

class CombineResolvers {
public:
  explicit CombineResolvers(CombineContext *);

  JsonDocument execute(JsonDocument);

private:
  CombineContext *context = nullptr;
  std::map<std::string, Query *> queryMap;
  std::map<std::string, Mutation *> mutationMap;

  void CombineQuery(void);

  void CombineMutation(void);
};

JsonDocument CombineResolvers::execute(JsonDocument json) {
  DynamicJsonDocument response(2048);
  uint32_t incomingTime = millis();
  std::string topic = json["topic"].as<String>().c_str();
  std::string method = json["method"].as<String>().c_str();

  if (method == "query") {
    if (queryMap.find(topic) != queryMap.end()) {
      try {
        JsonDocument data = queryMap[topic]->exec(json["data"], context);
        response["topic"] = json["topic"];
        response["method"] = json["method"];
        response["execTime"] = millis() - incomingTime;
        response["data"] = data;

        return response;
      }
      catch (ValidationError err) {
        JsonDocument errResponse = err.toJson();
        errResponse["execTime"] = millis() - incomingTime;
        return errResponse;
      }
    }

    TopicNotFoundError err;
    return err.toJson();
  }

  else if (method == "mutation") {
    if (mutationMap.find(topic) != mutationMap.end()) {
      try {
        JsonDocument data = mutationMap[topic]->exec(json["data"], context);
        response["topic"] = json["topic"];
        response["method"] = json["method"];
        response["execTime"] = millis() - incomingTime;
        response["data"] = data;

        return response;
      }
      catch (ValidationError err) {
        JsonDocument errResponse = err.toJson();
        errResponse["execTime"] = millis() - incomingTime;
        return errResponse;
      }
    }

    TopicNotFoundError err;
    return err.toJson();
  }

  InvalidMethodError err;
  return err.toJson();
}

CombineResolvers::CombineResolvers(CombineContext *context) : context(context) {
  CombineQuery();
  CombineMutation();
}

void CombineResolvers::CombineQuery() {
  auto *info = new query_info;
  queryMap[info->getName()] = info;

  auto *channel = new query_channel;
  queryMap[channel->getName()] = channel;

  auto *channel_state = new query_channel_state;
  queryMap[channel_state->getName()] = channel_state;

  auto *criteria = new query_criteria;
  queryMap[criteria->getName()] = criteria;

  auto *range = new query_range;
  queryMap[criteria->getName()] = range;

  auto *timer = new query_timer;
  queryMap[timer->getName()] = timer;

  auto *continuous = new query_continuous;
  queryMap[continuous->getName()] = continuous;

  auto *continuous_criteria = new query_continuous_criteria;
  queryMap[continuous_criteria->getName()] = continuous_criteria;

  auto *control_type = new query_control_type;
  queryMap[control_type->getName()] = control_type;

  auto *gpio_state = new query_gpio_state;
  queryMap[gpio_state->getName()] = gpio_state;

  auto *gpio_task = new query_gpio_task;
  queryMap[gpio_task->getName()] = gpio_task;

  auto *nsensor = new query_nsensor;
  queryMap[nsensor->getName()] = nsensor;

  auto *nsensors = new query_nsensors;
  queryMap[nsensors->getName()] = nsensors;

  auto *precondition_criteria = new query_precondition_criteria;
  queryMap[precondition_criteria->getName()] = precondition_criteria;

  auto *precondition_range = new query_precondition_range;
  queryMap[precondition_range->getName()] = precondition_range;

  auto *precondition_timer = new query_precondition_timer;
  queryMap[precondition_timer->getName()] = precondition_timer;

  auto *date = new query_date;
  queryMap[date->getName()] = date;

  auto *timezone = new query_timezone;
  queryMap[timezone->getName()] = timezone;

  auto *sensor = new query_sensor;
  queryMap[sensor->getName()] = sensor;

  auto *sensor_order = new query_sensor_order;
  queryMap[sensor_order->getName()] = sensor_order;


}

void CombineResolvers::CombineMutation() {
  auto *channel_activate = new mutation_channel_activate;
  mutationMap[channel_activate->getName()] = channel_activate;

  auto *channel_save = new mutation_channel_save;
  mutationMap[channel_save->getName()] = channel_save;

  auto *criteria_save = new mutation_criteria_save;
  mutationMap[criteria_save->getName()] = criteria_save;

  auto *range_save = new mutation_range_save;
  mutationMap[range_save->getName()] = range_save;

  auto *timer_save = new mutation_timer_save;
  mutationMap[timer_save->getName()] = timer_save;

  auto *continuous_criteria_save = new mutation_continuous_criteria_save;
  mutationMap[continuous_criteria_save->getName()] = continuous_criteria_save;

  auto *continuous_save = new mutation_continuous_save;
  mutationMap[continuous_save->getName()] = continuous_save;

  auto *control_type_save = new mutation_control_type_save;
  mutationMap[control_type_save->getName()] = control_type_save;

  auto *clear_nvmemory = new mutation_clear_nvmemory;
  mutationMap[clear_nvmemory->getName()] = clear_nvmemory;

  auto *precondition_criteria_save = new mutation_precondition_criteria_save;
  mutationMap[precondition_criteria_save->getName()] = precondition_criteria_save;

  auto *precondition_range_save = new mutation_precondition_range_save;
  mutationMap[precondition_range_save->getName()] = precondition_range_save;

  auto *precondition_timer_save = new mutation_precondition_timer_save;
  mutationMap[precondition_timer_save->getName()] = precondition_timer_save;

  auto *date_save = new mutation_date_save;
  mutationMap[date_save->getName()] = date_save;

  auto *timezone_save = new mutation_timezone_save;
  mutationMap[timezone_save->getName()] = timezone_save;

  auto *sensor_order_save = new mutation_sensor_order_save;
  mutationMap[sensor_order_save->getName()] = sensor_order_save;

  auto *continuous_activate = new mutation_continuous_activate;
  mutationMap[continuous_activate->getName()] = continuous_activate;


}