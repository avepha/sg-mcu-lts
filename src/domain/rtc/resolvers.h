//
// Created by Alfarie-MBP on 2019-08-08.
//
#include "../resolvers.h"
#include "combineContext.h"

#include "util/RtcUtil.h"

#ifndef SG_MCU_RTC_RESOLVERS_H
#define SG_MCU_RTC_RESOLVERS_H

// date_save
// date

class DateSave: public Resolvers {
};

// @Query
class DateResolvers: public Resolvers {
public:
  DateResolvers() : Resolvers("date"){};
  String resolve(CombineContext *context) {
    DateTime dateTime = context->rtcContext->core->getDate();

    StaticJsonDocument<1024> json;
    String jsonString;
    json["topic"] = "date";
    json["method"] = "query";
    json["data"] = DateTimeToIsoString(dateTime);

    serializeJson(json, jsonString);
    return jsonString;
  };
};

#endif //SG_MCU_RESOLVERS_H
