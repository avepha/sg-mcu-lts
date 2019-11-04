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
    if (queryMap.find(topic) !=  queryMap.end()) {
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
  auto *info = new query_info; queryMap[info->getName()] = info;

  auto *channel = new query_channel; queryMap[channel->getName()] = channel;
  auto *channel_state = new query_channel_state; queryMap[channel_state->getName()] = channel_state;
  auto *criteria = new query_criteria; queryMap[criteria->getName()] = criteria;
  auto *range = new query_range; queryMap[criteria->getName()] = range;
  auto *timer = new query_timer; queryMap[timer->getName()] = timer;

}

void CombineResolvers::CombineMutation() {
  auto *channel_activate = new mutation_channel_activate; mutationMap[channel_activate->getName()] = channel_activate;
  auto *channel_save = new mutation_channel_save; mutationMap[channel_save->getName()] = channel_save;
  auto *criteria_save = new mutation_criteria_save; mutationMap[criteria_save->getName()] = criteria_save;
  auto *range_save = new mutation_range_save; mutationMap[range_save->getName()] = range_save;
  auto *timer_save = new mutation_timer_save; mutationMap[timer_save->getName()] = timer_save;

}