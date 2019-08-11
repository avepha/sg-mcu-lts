//
// Created by Alfarie-MBP on 2019-08-11.
//

#ifndef SG_MCU_INIT_H
#define SG_MCU_INIT_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Unity.h>
#include <RTClib.h>

#include "../src/util/JsonTopic.h"
#include "../src/combineContext.h"
#include "../src/combineResolvers.h"

CombineContext context;
CombineResolvers resolvers(&context);

#endif //SG_MCU_INIT_H
