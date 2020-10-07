#include "domain/permission.h"
#include "validationError.h"

#ifndef SG_MCU_PSTATION_PERMISSION_H
#define SG_MCU_PSTATION_PERMISSION_H

class permission_pstation_remove : public Permission {
public:
  explicit permission_pstation_remove() : Permission() {};

  void resolve(JsonObject reqData, CombineContext *context) override {
    if (reqData["address"].isNull()) {
      InvalidInputError err("Address must be specified");
      throw err;
    }

    if (!reqData["address"].is<int>()) {
      InvalidInputError err("Address must be an integer");
      throw err;
    }

    uint8_t address = reqData["address"];
    bool flag = false;
    PStationSchema model = context->pstation->model->get();
    for (int i = 0; i < model.stationSize; i++) {
      if (model.stations[i].address == address) {
        flag = true;
      }
    }
    if (!flag) {
      InvalidInputError err("Address not found");
      throw err;
    }
  }
};

class permission_pstation_add : public Permission {
public:
  explicit permission_pstation_add() : Permission() {};

  void resolve(JsonObject reqData, CombineContext *context) override {
    PStationSchema model = context->pstation->model->get();
    if (model.stationSize >= 8) {
      InvalidInputError err("Station size exceeded");
      throw err;
    }
    if (reqData["address"].isNull()) {
      InvalidInputError err("Address must be specified");
      throw err;
    }
    if (!reqData["address"].is<int>()) {
      InvalidInputError err("Address must be an integer");
      throw err;
    }

   if (reqData["enable"].isNull()) {
      InvalidInputError err("Enable must be specified");
      throw err;
    }
    if (!reqData["enable"].is<bool>()) {
      InvalidInputError err("Enable must be a boolean");
      throw err;
    }

    if (reqData["type"].isNull()) {
      InvalidInputError err("Type must be specified");
      throw err;
    }
    if (!reqData["type"].is<String>()) {
      InvalidInputError err("Type must be a string");
      throw err;
    }
    if(PStationTypeStringToEnum(reqData["type"]) == PSTATION_NULL) {
      InvalidInputError err("Unknown type error");
      throw err;
    }

    if (reqData["sensors"].isNull()) {
      InvalidInputError err("Sensors must be specified");
      throw err;
    }
    if (!reqData["sensors"].is<JsonArray>()) {
      InvalidInputError err("Sensors must be an array of integer");
      throw err;
    }
    if (reqData["sensors"].size() >= 10) {
      InvalidInputError err("Sensors size must less than 10 ");
      throw err;
    }

    for (int i = 0 ; i < reqData["sensors"].size(); i++) {
      if (!reqData["sensors"][i].is<int>()) {
        InvalidInputError err("Sensors must be an array of integer");
        throw err;
      }
    }
  }
};

#endif //SG_MCU_PERMISSION_H
