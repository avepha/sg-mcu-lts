#include <domain/sensor/resolvers.h>
#include "combineContext.h"
#include "validationError.h"

#include "domain/rtc/resolvers.h"
#include "domain/sensor/resolvers.h"
#include "domain/nvmemory/resolvers.h"
#include "domain/precondition/resolvers.h"

class CombineResolvers
{
public:
  explicit CombineResolvers(CombineContext *);
  String execute(JsonObject);

private:
  static const int QUERY_SIZE = 4;
  static const int MUTATION_SIZE = 3;
  CombineContext *context;
  Resolvers *mutation[MUTATION_SIZE];
  Resolvers *query[QUERY_SIZE];
};

CombineResolvers::CombineResolvers(CombineContext *context):
  context(context)
  {
    // TODO: revise data structure, turn array to map
    query[0] = new query_date(context);
    query[1] = new query_sensor(context);
    query[2] = new query_sensor_order(context);
    query[3] = new query_precondition(context);

    mutation[0] = new mutation_date_save(context);
    mutation[1] = new mutation_sensor_order_save(context);
    mutation[2] = new mutation_clear_nvmemory(context);
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