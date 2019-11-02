#include "./preconditionTypeEnum.h"
#ifndef SG_MCU_PRECONDITION_H
#define SG_MCU_PRECONDITION_H

class Precondition {
public:
  explicit Precondition(PRECONDITION_TYPE_ENUM type) : type(type) {}
  virtual ~Precondition() = default;
  virtual bool resolve() {
    return false;
  };

  PRECONDITION_TYPE_ENUM getType() {
    return type;
  }

private:
  PRECONDITION_TYPE_ENUM type;
};

#endif //SG_MCU_PRECONDITION_H
