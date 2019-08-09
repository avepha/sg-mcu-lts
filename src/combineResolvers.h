#include "combineContext.h"
#include "./domain/sensor/resolvers.h"
#include "./domain/control/resolvers.h"
#include "./domain/rtc/resolvers.h"

class CombineResolvers
{
public:
  explicit CombineResolvers(CombineContext *);
  String execute(JsonObject);

private:
  CombineContext *context;
  SensorResolvers sensorResolvers;
  ControlResolvers controlResolvers;
  DateResolvers dateResolvers;
};

CombineResolvers::CombineResolvers(CombineContext *context):
  context(context)
  {};


String CombineResolvers::execute(JsonObject json) {

}