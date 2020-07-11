#ifndef SG_MCU_LTS_PRESETENUM_H
#define SG_MCU_LTS_PRESETENUM_H

enum SgModelEnum {
  MODEL_GENERAL = 1,
  MODEL_GSOLUTION = 2,
  MODEL_NONE = -1,
};

String ModelEnumToString(SgModelEnum model) {
  switch(model) {
    case MODEL_GENERAL:
      return "general";
    case MODEL_GSOLUTION:
      return "gsolution";
    default:
      return "none";
  }
}

SgModelEnum ModelStringToEnum(String strModel) {
  if (strModel == "general") return MODEL_GENERAL;
  else if (strModel == "gsolution") return MODEL_GSOLUTION;
  else return MODEL_NONE;
}

#endif //SG_MCU_LTS_PRESETENUM_H
