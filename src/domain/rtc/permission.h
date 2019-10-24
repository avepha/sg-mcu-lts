#include "domain/permission.h"
#include "validationError.h"

#ifndef SG_MCU_RTC_PERMISSION_H
#define SG_MCU_RTC_PERMISSION_H

class permission_timezone_save : public Permission {
public:
  explicit permission_timezone_save(CombineContext *context) : Permission(context) {};

  void resolve(JsonObject reqData) override {
    if (reqData["offset"].isNull()) {
      InvalidInputError err("offset is not specified.");
      throw err;
    }

    if (!reqData["offset"].is<String>()) {
      InvalidInputError err("offset must be string.");
      throw err;
    }

    String offset = reqData["offset"].as<String>();
    if (offset.indexOf(':') != 3 || offset.length() != 6) {
      InvalidInputError err("offset must be format {+hh:mm}.");
      throw err;
    };
  }
};

#endif //SG_MCU_PERMISSION_H
