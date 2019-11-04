#ifndef SG_MCU_INIT_H
#define SG_MCU_INIT_H
#define _TASK_OO_CALLBACKS
#define _TASK_PRIORITY

#include <unity.h>
#include <Arduino.h>
#include <Wire.h>
#include <EEPROM.h>
#include "config.h"
#include <TaskScheduler.h>
#include <ArduinoJson.h>

#define VERSION "0.2.4"
#define PROJECT "SMART-FARM"
#define UPLOADDATE String(__DATE__) + " " + String(__TIME__)
#define COMCORE 0
#define MAINCORE 1
#define EEPROM_SIZE 4096

Scheduler controlScheduler, gpioScheduler;

#include "util/util.h"
#include "combineContext.h"
#include "combineResolvers.h"

#include "validationError.h"
#include "deviceEndpoint.h"
#include "sensorEndpoint.h"

CombineResolvers *resolvers;
CombineContext *context;

#endif //SG_MCU_INIT_H
