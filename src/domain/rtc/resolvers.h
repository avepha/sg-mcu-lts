//
// Created by Alfarie-MBP on 2019-08-08.
//
#include <RTClib.h>
#include "../resolvers.h"
#include "validationError.h"
#include "combineContext.h"
#include "util/RtcUtil.h"

#ifndef SG_MCU_RTC_RESOLVERS_H
#define SG_MCU_RTC_RESOLVERS_H

// @mutation: date_save
class mutation_date_save : public Resolvers {
public:
  explicit mutation_date_save(CombineContext *context) : Resolvers("date_save", context) {};

  String resolve(JsonObject json) override {
    if (json["data"]["date"].isNull()) {
      return (new InvalidInputError("Date is not specified"))->toJsonString();
    }

    DateTime newDate(IsoStringToDateTime(json["data"]["date"]));

    context->rtcContext->core->setDate(newDate);
    DateTime dateTime = context->rtcContext->core->getDate();

    StaticJsonDocument<1024> result;
    String jsonString;
    result["topic"] = "date_save";
    result["method"] = "mutation";
    result["data"] = DateTimeToIsoString(dateTime);

    serializeJson(json, jsonString);
    return jsonString;
  };
};

// @query: date
class query_date : public Resolvers {
public:
  explicit query_date(CombineContext *context) : Resolvers("date", context) {};

  String resolve(JsonObject  reqJson) override {
    DateTime dateTime = context->rtcContext->core->getDate();

    StaticJsonDocument<1024> json;
    String jsonString;
    json["topic"] = reqJson["topic"];
    json["method"] = reqJson["method"];
    json["data"] = DateTimeToIsoString(dateTime);

    serializeJson(json, jsonString);
    return jsonString;
  };
};

#endif //SG_MCU_RESOLVERS_H
