#include "combineContext.h"
#include "./domain/sensor/resolvers.h"
#include "./domain/control/resolvers.h"
#include "./domain/rtc/resolvers.h"

class CombineResolvers
{
public:
  SensorResolvers sensorResolvers;
  ControlResolvers controlResolvers;
  DateResolvers dateResolvers;
  explicit CombineResolvers(CombineContext *);
  String execute(JsonObject);

private:
  CombineContext *context;
};

CombineResolvers::CombineResolvers(CombineContext *context):
  context(context)
  {};