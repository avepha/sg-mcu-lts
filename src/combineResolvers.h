#include "combineContext.h"
#include "validationError.h"

#include "domain/rtc/resolvers.h"
#include "domain/sensor/resolvers.h"
#include "domain/nsensor/resolvers.h"
#include "domain/nvmemory/resolvers.h"
#include "domain/precondition/resolvers.h"
#include "domain/control/criteria/resolvers.h"
#include "domain/control/timer/resolvers.h"
#include "domain/info/resolvers.h"

class CombineResolvers
{
public:
  explicit CombineResolvers(CombineContext *);
  String execute(JsonObject);

private:
  static const int QUERY_SIZE = 9;
  static const int MUTATION_SIZE = 6;
  CombineContext *context;
  Resolvers *mutation[MUTATION_SIZE];
  Resolvers *query[QUERY_SIZE];
};

CombineResolvers::CombineResolvers(CombineContext *context):
  context(context)
  {
    query[0] = new query_date(context);
    query[1] = new query_sensor(context);
    query[2] = new query_sensor_order(context);
    query[3] = new query_precondition(context);
    query[4] = new query_criteria(context);
    query[5] = new query_timer(context);
    query[6] = new query_nsensors(context);
    query[7] = new query_nsensor(context);
    query[8] = new query_info(context);

    mutation[0] = new mutation_date_save(context);
    mutation[1] = new mutation_sensor_order_save(context);
    mutation[2] = new mutation_clear_nvmemory(context);
    mutation[3] = new mutation_precondition_save(context);
    mutation[4] = new mutation_criteria_save(context);
    mutation[5] = new mutation_timer_save(context);
  };


String CombineResolvers::execute(JsonObject json) {

  if (json["method"] == "query") {
    for(int i = 0 ; i < QUERY_SIZE; i++) {
      if (query[i]->getName() == json["topic"]) {
        return query[i]->resolve(json);
      }
    }
  }
  else if (json["method"] == "mutation") {
    for(int i = 0 ; i < MUTATION_SIZE; i++) {
      if (mutation[i]->getName() == json["topic"]) {
        return mutation[i]->resolve(json);
      }
    }
  }

  return (new InvalidMethodError())->toJsonString();
}