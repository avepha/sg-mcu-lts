//
// Created by Alfarie-MBP on 2019-08-08.
//
#include "../resolvers.h"
#ifndef SG_CONTROL_RESOLVERS_H
#define SG_CONTROL_RESOLVERS_H


class ControlResolvers : public Resolvers {
public:
  float control_1;
  ControlResolvers();
};

ControlResolvers::ControlResolvers():
  Resolvers("control"),
  control_1(1.0) {};

#endif //SG_CONTROL_RESOLVERS_H
