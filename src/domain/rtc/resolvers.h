#include <RTClib.h>

#include "validationError.h"
#include "combineContext.h"
#include "util/util.h"
#include "./permission.h"

#include "./util/RtcUtil.h"

#ifndef SG_MCU_RTC_RESOLVERS_H
#define SG_MCU_RTC_RESOLVERS_H

// TODO: check iso string
// @mutation: date_save
class mutation_date_save : public Mutation {
public:
  explicit mutation_date_save() : Mutation("date_save") {};

  JsonDocument resolve(JsonObject reqData, CombineContext *context) override {
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

class mutation_timezone_save : public Mutation {
public:
  explicit mutation_timezone_save() : Mutation("timezone_save", new permission_timezone_save()) {};

  JsonDocument resolve(JsonObject reqData, CombineContext *context) override {
    String offset = reqData["offset"];
    String sign = offset.substring(0, 1);
    String hourOffset = offset.substring(1, 3);
    String minOffset = offset.substring(4);
    RtcSchema schema = context->rtc->model->get();

    schema.tzOffsetMin = minOffset.toInt();
    schema.tzOffsetHour = (sign == "+") ? hourOffset.toInt() : hourOffset.toInt() * -1;

    context->rtc->model->save(schema);

    RtcSchema newSchema = context->rtc->model->get();
    DynamicJsonDocument data(64);

    String tzString = (newSchema.tzOffsetHour >= 0) ? "+" : "-";
    int hour = (newSchema.tzOffsetHour < 0) ? newSchema.tzOffsetHour * -1 : newSchema.tzOffsetHour;
    tzString += (hour > 10) ? String(hour) : "0" + String(hour);
    tzString += ":";
    tzString += (newSchema.tzOffsetMin > 10) ? String(newSchema.tzOffsetMin) : "0" + String(newSchema.tzOffsetMin);

    data["offset"] = tzString;
    return data;
  };
};

// @query: date
class query_date : public Query {
public:
  explicit query_date() : Query("date") {};

  JsonDocument resolve(JsonObject reqData, CombineContext *context) override {
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

class query_timezone : public Query {
public:
  explicit query_timezone() : Query("timezone") {};

  JsonDocument resolve(JsonObject reqData, CombineContext *context) override {

    RtcSchema schema = context->rtc->model->get();
    DynamicJsonDocument data(64);

    String tzString = (schema.tzOffsetHour >= 0) ? "+" : "-";
    int hour = (schema.tzOffsetHour < 0) ? schema.tzOffsetHour * -1 : schema.tzOffsetHour;
    tzString += (hour > 10) ? String(hour) : "0" + String(hour);
    tzString += ":";
    tzString += (schema.tzOffsetMin > 10) ? String(schema.tzOffsetMin) : "0" + String(schema.tzOffsetMin);

    data["offset"] = tzString;
    return data;
  };
};


#endif //SG_MCU_RESOLVERS_H
