#include "domain/permission.h"
#include "validationError.h"

#ifndef SG_MCU_PAR_PERMISSION_H
#define SG_MCU_PAR_PERMISSION_H

class permission_par_save : public Permission {
public:
  explicit permission_par_save() : Permission() {};

  void resolve(JsonObject reqData, CombineContext *context) override {
    if (reqData.isNull() || reqData["index"].isNull()) {
      InvalidInputError err("index is not specified.");
      throw err;
    }

    if (reqData["index"] < 0 || reqData["index"] > 7) {
      InvalidInputError err("index out of range.");
      throw err;
    }

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

class permission_par : public Permission {
public:
  explicit permission_par() : Permission() {};

  void resolve(JsonObject reqData, CombineContext *context) override {
    if (reqData.isNull() || reqData["index"].isNull()) {
      InvalidInputError err("index must not be null");
      throw err;
    }

    if (reqData["index"] < 0 || reqData["index"] > 7) {
      InvalidInputError err("index out of range.");
      throw err;
    }
  }
};

#endif //SG_MCU_PAR_PERMISSION_H
