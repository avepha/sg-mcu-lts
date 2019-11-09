#include "domain/permission.h"
#include "validationError.h"

#ifndef SG_MCU_SEQUENCE_RANGE_PERMISSION_H
#define SG_MCU_SEQUENCE_RANGE_PERMISSION_H

class permission_sequence_range_save : public Permission {
public:
  explicit permission_sequence_range_save() : Permission() {};

  void resolve(JsonObject reqData, CombineContext *context) override {
    if (reqData["range"].isNull()) {
      InvalidInputError err("range field must not be null");
      throw err;
    }

    if (!reqData["range"].is<JsonObject>()) {
      InvalidInputError err("range field must be an object");
      throw err;
    }

    JsonObject range = reqData["range"];
    if (range["sensor"].isNull()) {
      InvalidInputError err("range.sensor field must not be null");
      throw err;
    }

    if (!range["sensor"].is<int>()) {
      InvalidInputError err("range.sensor field must be a number");
      throw err;
    }

    if (range["upper"].isNull()) {
      InvalidInputError err("range.upper field must not be null");
      throw err;
    }

    if (!range["upper"].is<float>()) {
      InvalidInputError err("range.upper field must be a number");
      throw err;
    }

    if (range["lower"].isNull()) {
      InvalidInputError err("range.upper field must not be null");
      throw err;
    }

    if (!range["lower"].is<float>()) {
      InvalidInputError err("range.lower field must be a number");
      throw err;
    }

    if (range["greater"].isNull()) {
      InvalidInputError err("range.greater field must not be null");
      throw err;
    }

    if (!range["greater"].is<bool>()) {
      InvalidInputError err("range.lower field must be a boolean");
      throw err;
    }

    if (!range["timing"].isNull()) {
      JsonObject timing = range["timing"];
      if (timing["waiting"].isNull()) {
        InvalidInputError err("range.greater field must not be null");
        throw err;
      }

      if (!timing["waiting"].is<int>()) {
        InvalidInputError err("range.lower field must be a number");
        throw err;
      }
    }
  }
};

#endif //SG_MCU_SEQUENCE_RANGE_PERMISSION_H
