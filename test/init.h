#ifndef SG_MCU_INIT_H
#define SG_MCU_INIT_H
#define _TASK_OO_CALLBACKS

#define VERSION "0.0.1"
#define EEPROM_SIZE 4096

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Unity.h>
#include <RTClib.h>

#include "../src/config.h"
#include "util/JsonRequest.h"
#include "../src/combineContext.h"
#include "../src/combineResolvers.h"

CombineContext *context;
CombineResolvers *resolvers;

#endif //SG_MCU_INIT_H
