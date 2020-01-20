#include "domain/permission.h"
#include "validationError.h"

#ifndef SG_MCU_STATION_PERMISSION_H
#define SG_MCU_STATION_PERMISSION_H

class permission_station : public Permission {
public:
  explicit permission_station() : Permission() {};

  void resolve(JsonObject reqData, CombineContext *context) override {
    if (reqData["address"].isNull()) {
      InvalidInputError err("address must not be null");
      throw err;
    }

    if (!reqData["address"].is<int>()) {
      InvalidInputError err("address must be an integer");
      throw err;
    }

    byte address = reqData["address"];
    bool flag = false;
    std::vector<Station*> stations = context->station->core->getStations();
    for (auto station: stations) {
      if (station->getAddress() == address) {
        flag = true;
      }
    }
    if (!flag) {
      StationNotFoundError err;
      throw err;
    }
  }
};

#endif //SG_MCU_CONTROL_PERMISSION_H
