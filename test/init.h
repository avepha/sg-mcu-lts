//
// Created by Alfarie-MBP on 2019-08-11.
//

#ifndef SG_MCU_INIT_H
#define SG_MCU_INIT_H

#define VERSION "0.0.1"
#define EEPROM_SIZE 4096

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Unity.h>
#include <RTClib.h>

#include "util/JsonRequest.h"
#include "../src/combineContext.h"
#include "../src/combineResolvers.h"

CombineContext context;
CombineResolvers resolvers(&context);

#endif //SG_MCU_INIT_H
