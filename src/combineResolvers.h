#include "combineContext.h"
#include "validationError.h"

#include "domain/rtc/resolvers.h"
#include "domain/sensor/resolvers.h"
#include "domain/nsensor/resolvers.h"
#include "domain/nvmemory/resolvers.h"
#include "domain/control/criteria/resolvers.h"
#include "domain/control/timer/resolvers.h"
#include "domain/info/resolvers.h"
#include "domain/channel/resolvers.h"
#include "domain/precondition/criteria/resolvers.h"
#include "domain/precondition/timer/resolvers.h"

class CombineResolvers {
public:
  explicit CombineResolvers(CombineContext *);

  JsonDocument execute(JsonDocument);

private:
  static const int QUERY_SIZE = 13;
  static const int MUTATION_SIZE = 9;
  CombineContext *context;
  Resolvers *mutation[MUTATION_SIZE];
  Resolvers *query[QUERY_SIZE];
};

CombineResolvers::CombineResolvers(CombineContext *context) : context(context) {
  query[0] = new query_date(context);
  query[1] = new query_sensor(context);
  query[2] = new query_sensor_order(context);
  query[3] = new query_criteria(context);
  query[4] = new query_timer(context);
  query[5] = new query_nsensors(context);
  query[6] = new query_nsensor(context);
  query[7] = new query_info(context);
  query[8] = new query_channel(context);
  query[9] = new query_gpio(context);
  query[10] = new query_channel_state(context);
  query[11] = new query_precondition_criteria(context);
  query[12] = new query_precondition_timer(context);

  mutation[0] = new mutation_date_save(context);
  mutation[1] = new mutation_sensor_order_save(context);
  mutation[2] = new mutation_clear_nvmemory(context);
  mutation[3] = new mutation_criteria_save(context);
  mutation[4] = new mutation_timer_save(context);
  mutation[5] = new mutation_channel_save(context);
  mutation[6] = new mutation_channel_activate(context);
  mutation[7] = new mutation_precondition_criteria_save(context);
  mutation[8] = new mutation_precondition_timer_save(context);
};


JsonDocument CombineResolvers::execute(JsonDocument json) {
  DynamicJsonDocument response(2048);
  uint32_t incomingTime = millis();

  if (json["method"] == "query") {
    for (int i = 0; i < QUERY_SIZE; i++) {
      if (query[i]->getName() == json["topic"]) {
        try {
          JsonDocument data = query[i]->exec(json["data"]);
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
    }

    TopicNotFoundError err;
    return err.toJson();
  }
  else if (json["method"] == "mutation") {
    for (int i = 0; i < MUTATION_SIZE; i++) {
      if (mutation[i]->getName() == json["topic"]) {
        try {
          JsonDocument data = mutation[i]->exec(json["data"]);
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
    }

    TopicNotFoundError err;
    return err.toJson();
  }

  InvalidMethodError err;
  return err.toJson();
}