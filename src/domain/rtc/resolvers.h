#include <RTClib.h>

#include "validationError.h"
#include "combineContext.h"
#include "domain/resolvers.h"
#include "util/util.h"

#include "./util/RtcUtil.h"

#ifndef SG_MCU_RTC_RESOLVERS_H
#define SG_MCU_RTC_RESOLVERS_H

// TODO: check iso string
// @mutation: date_save
class mutation_date_save : public Resolvers {
public:
  explicit mutation_date_save(CombineContext *context) : Resolvers("date_save", context) {};

  JsonDocument resolve(JsonObject reqData) override {
    if (reqData["date"].isNull()) {
      InvalidInputError err;
      throw err;
    }

    DateTime newDate(IsoStringToDateTime(reqData["date"]));
    context->rtc->core->setDate(newDate);
    DateTime dateTime = context->rtc->core->getDate();

    DynamicJsonDocument data(64);
    data["date"] = DateTimeToIsoString(dateTime);

    return data;
  };
};

// @query: date
class query_date : public Resolvers {
public:
  explicit query_date(CombineContext *context) : Resolvers("date", context) {};

  JsonDocument resolve(JsonObject reqData) override {
    DateTime dateTime = context->rtc->core->getDate();

    DynamicJsonDocument data(64);
    data["date"] = DateTimeToIsoString(dateTime);

    return data;
  };
};

#endif //SG_MCU_RESOLVERS_H
