#include "domain/model.h"

#ifndef SG_MCU_CONTROL_CRITERIA_MODEL_H
#define SG_MCU_CONTROL_CRITERIA_MODEL_H

const int CRITERIA_ROM_ADDR = 700;

struct CriteriaSchema {
  struct Criteria {
    byte sensor = 0;
    float criteria = 0.0F;
    bool greater = false;
    struct Timing {
      bool enable = false;
      uint16_t workingTimeInSecond = 10;
      uint16_t waitingTimeInSecond = 10;
    } timing;
  } criterias[8];
}; // total size: 128

class CriteriaModel : public RomModel<CriteriaSchema> {
public:
  CriteriaModel() : RomModel(CRITERIA_ROM_ADDR, "CriteriaModel") {};

  static void ShowModel(CriteriaSchema::Criteria _criteria, int channel) {
    DynamicJsonDocument data(256);
    data["index"] = channel;
    JsonObject criteria = data.createNestedObject("criteria");
    criteria["sensor"] = _criteria.sensor;
    criteria["criteria"] = _criteria.criteria;
    criteria["greater"] = _criteria.greater;

    JsonObject timing = criteria.createNestedObject("timing");
    timing["enable"] = _criteria.timing.enable;
    timing["working"] = _criteria.timing.workingTimeInSecond;
    timing["waiting"] = _criteria.timing.waitingTimeInSecond;

    serializeJsonPretty(data, Serial);
  }
};

#endif //SG_MCU_CRITERIA_MODEL_H
