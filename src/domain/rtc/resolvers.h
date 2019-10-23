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
    DateTime dateTime = context->rtc->core->getUtcDate();

    DynamicJsonDocument data(64);
    data["date"] = DateTimeToIsoString(dateTime);

    return data;
  };
};

class mutation_timezone_save: public Resolvers {
public:
  explicit mutation_timezone_save(CombineContext *context) : Resolvers("timezone_save", context) {};

  JsonDocument resolve(JsonObject reqData) override {
    if (reqData["timezone"].isNull()) {
      InvalidInputError err("timezone must not be null");
      throw err;
    }

    if (!reqData["timezone"].is<int>()) {
      InvalidInputError err("timezone must not be a number");
      throw err;
    }

    RtcSchema schema = context->rtc->model->get();
    schema.timezone = reqData["timezone"];
    context->rtc->model->save(schema);
    delay(10);

    RtcSchema newSchema = context->rtc->model->get();
    DynamicJsonDocument data(64);
    data["timezone"] = newSchema.timezone;
    return data;
  };
};

// @query: date
class query_date : public Resolvers {
public:
  explicit query_date(CombineContext *context) : Resolvers("date", context) {};

  JsonDocument resolve(JsonObject reqData) override {
    DateTime dateTime;
    if (!reqData["local"].isNull()) {
      dateTime = context->rtc->core->getDate();
    }
    else {
      dateTime = context->rtc->core->getUtcDate();
    }

    DynamicJsonDocument data(64);
    data["date"] = DateTimeToIsoString(dateTime);

    return data;
  };
};

class query_timezone : public Resolvers {
public:
  explicit query_timezone(CombineContext *context) : Resolvers("timezone", context) {};

  JsonDocument resolve(JsonObject reqData) override {
    RtcSchema schema = context->rtc->model->get();

    DynamicJsonDocument data(64);
    data["timezone"] = schema.timezone;

    return data;
  };
};


#endif //SG_MCU_RESOLVERS_H
