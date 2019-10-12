#ifndef SG_MCU_PRECONDITION_H
#define SG_MCU_PRECONDITION_H

enum PRECONDITION_TYPE_ENUM {
  PREC_NONE = 0,
  PREC_TIMER = 1,
  PREC_CRITERIA = 2,
  PREC_RANGE = 3
};

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
