#include "./resolvers.h"

#ifndef SG_MCU_MUTATION_H
#define SG_MCU_MUTATION_H

class Mutation : public Resolvers {
public:
  Mutation(const std::string &name, Permission *permission = nullptr) : Resolvers(name, permission) {}
};


#endif //SG_MCU_MUTATION_H
