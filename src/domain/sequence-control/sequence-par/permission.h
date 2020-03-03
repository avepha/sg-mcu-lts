#include "domain/permission.h"
#include "validationError.h"

#ifndef SG_MCU_SEQUENCE_PAR_PERMISSION_H
#define SG_MCU_SEQUENCE_PAR_PERMISSION_H

class permission_sequence_par_save : public Permission {
public:
  explicit permission_sequence_par_save() : Permission() {};

  void resolve(JsonObject reqData, CombineContext *context) override {
    if (reqData["par"].isNull()) {
      InvalidInputError err("par must not be null.");
      throw err;
    }

    if (!reqData["par"].is<JsonObject>()) {
      InvalidInputError err("par must be an object.");
      throw err;
    }

    JsonObject par = reqData["par"];
    if (par["parSumInKJ"].isNull()) {
      InvalidInputError err("parSumInKJ must not be null.");
      throw err;
    }

    if (!par["parSumInKJ"].is<int>()) {
      InvalidInputError err("sensor must be type of integer");
      throw err;
    }

    if (par["timing"].isNull()) {
      InvalidInputError err("timing must not be null");
      throw err;
    }

    if (par["timing"]["working"].isNull()) {
      InvalidInputError err("timing.working must not be null.");
      throw err;
    }
  }
};

#endif //SG_MCU_SEQUENCE_PAR_PERMISSION_H
