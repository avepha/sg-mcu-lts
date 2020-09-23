#include "combineContext.h"
#include "validationError.h"

#include "domain/mutation.h"
#include "domain/query.h"

#include "domain/configuration/resolvers.h"
#include "domain/rtc/resolvers.h"
#include "domain/nvmemory/resolvers.h"
#include "domain/info/resolvers.h"

#include "domain/rtc/resolvers.h"
#include "domain/gpio/resolvers.h"

#include "domain/control/resolvers.h"

#include "domain/channel/resolvers.h"
#include "domain/channel-control/criteria/resolvers.h"
#include "domain/channel-control/timer/resolvers.h"
#include "domain/channel-control/range/resolvers.h"
#include "domain/channel-control/par/resolvers.h"

#include "domain/sequence/resolvers.h"
#include "domain/sequence-control/sequence-criteria/resolvers.h"
#include "domain/sequence-control/sequence-timer/resolvers.h"
#include "domain/sequence-control/sequence-range/resolvers.h"
#include "domain/sequence-control/sequence-par/resolvers.h"

#include "domain/precondition/criteria/resolvers.h"
#include "domain/precondition/timer/resolvers.h"
#include "domain/precondition/range/resolvers.h"

#include "domain/station/resolvers.h"
#include "domain/sensor/resolvers.h"

#include "domain/notification/resolvers.h"

class CombineResolvers {
public:
  explicit CombineResolvers(CombineContext *);

  DynamicJsonDocument execute(DynamicJsonDocument);

private:
  CombineContext *context = nullptr;
  std::map<std::string, Query *> queryMap;
  std::map<std::string, Mutation *> mutationMap;

  void CombineQuery(void);

  void CombineMutation(void);
};

DynamicJsonDocument CombineResolvers::execute(DynamicJsonDocument json) {
  DynamicJsonDocument response(2048);
  uint32_t incomingTime = millis();
  std::string topic = json["topic"].as<String>().c_str();
  std::string method = json["method"].as<String>().c_str();

  if (method == "query") {
    if (queryMap.find(topic) != queryMap.end()) {
      try {
        DynamicJsonDocument data = queryMap[topic]->exec(json["data"], context);
        response["topic"] = json["topic"];
        response["method"] = json["method"];
        response["execTime"] = millis() - incomingTime;
        response["data"] = data;

        return response;
      }
      catch (ValidationError err) {
        DynamicJsonDocument errResponse = err.toJson();
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
        DynamicJsonDocument data = mutationMap[topic]->exec(json["data"], context);
        response["topic"] = json["topic"];
        response["method"] = json["method"];
        response["execTime"] = millis() - incomingTime;
        response["data"] = data;

        return response;
      }
      catch (ValidationError err) {
        DynamicJsonDocument errResponse = err.toJson();
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

  auto *device_status = new query_device_status;
  queryMap[device_status->getName()] = device_status;

  auto *channel = new query_channel;
  queryMap[channel->getName()] = channel;

  auto *channel_state = new query_channel_state;
  queryMap[channel_state->getName()] = channel_state;

  auto *criteria = new query_criteria;
  queryMap[criteria->getName()] = criteria;

  auto *range = new query_range;
  queryMap[range->getName()] = range;

  auto *timer = new query_timer;
  queryMap[timer->getName()] = timer;

  auto *sequence = new query_sequence;
  queryMap[sequence->getName()] = sequence;

  auto *sequence_criteria = new query_sequence_criteria;
  queryMap[sequence_criteria->getName()] = sequence_criteria;

  auto *control_type = new query_control_type;
  queryMap[control_type->getName()] = control_type;

  auto *gpio_state = new query_gpio_state;
  queryMap[gpio_state->getName()] = gpio_state;

  auto *gpio_task = new query_gpio_task;
  queryMap[gpio_task->getName()] = gpio_task;

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

  auto *sequence_state = new query_sequence_state;
  queryMap[sequence_state->getName()] = sequence_state;

  auto *sequence_timer = new query_sequence_timer;
  queryMap[sequence_timer->getName()] = sequence_timer;

  auto *sequence_range = new query_sequence_range;
  queryMap[sequence_range->getName()] = sequence_range;

  auto *par = new query_par;
  queryMap[par->getName()] = par;

  auto *sequence_order = new query_sequence_order;
  queryMap[sequence_order->getName()] = sequence_order;

  auto *configuration = new query_configuration;
  queryMap[configuration->getName()] = configuration;

  auto *stations = new query_stations;
  queryMap[stations->getName()] = stations;

  auto *station = new query_station;
  queryMap[station->getName()] = station;

  auto *sensor = new query_sensor;
  queryMap[sensor->getName()] = sensor;

  auto *sensors = new query_sensors;
  queryMap[sensors->getName()] = sensors;

  auto *sequence_par = new query_sequence_par;
  queryMap[sequence_par->getName()] = sequence_par;


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

  auto *sequence_criteria_save = new mutation_sequence_criteria_save;
  mutationMap[sequence_criteria_save->getName()] = sequence_criteria_save;

  auto *sequence_save = new mutation_sequence_save;
  mutationMap[sequence_save->getName()] = sequence_save;

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

  auto *sequence_activate = new mutation_sequence_activate;
  mutationMap[sequence_activate->getName()] = sequence_activate;

  auto *sequence_timer_save = new mutation_sequence_timer_save;
  mutationMap[sequence_timer_save->getName()] = sequence_timer_save;

  auto *sequence_range_save = new mutation_sequence_range_save;
  mutationMap[sequence_range_save->getName()] = sequence_range_save;

  auto *par_save = new mutation_par_save;
  mutationMap[par_save->getName()] = par_save;

  auto *sequence_order_save = new mutation_sequence_order_save;
  mutationMap[sequence_order_save->getName()] = sequence_order_save;

  auto *log_level_save = new mutation_log_level_save;
  mutationMap[log_level_save->getName()] = log_level_save;

  auto *sequence_par_save = new mutation_sequence_par_save;
  mutationMap[sequence_par_save->getName()] = sequence_par_save;

  auto *notification_ack = new mutation_notification_ack;
  mutationMap[notification_ack->getName()] = notification_ack;


}