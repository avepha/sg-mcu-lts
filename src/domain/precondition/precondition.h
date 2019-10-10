#ifndef SG_MCU_PRECONDITION_H
#define SG_MCU_PRECONDITION_H

enum PRECONDITION_TYPE_ENUM {
  PREC_NONE,
  PREC_TIMER,
  PREC_CRITERIA,
};

class Precondition {
public:
  virtual bool resolve() {
    return false;
  };
};

#endif //SG_MCU_PRECONDITION_H
