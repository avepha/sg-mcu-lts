#include "./resolvers.h"

#ifndef SG_MCU_QUERY_H
#define SG_MCU_QUERY_H

class Query : public Resolvers {
public:
  Query(const std::string &, Permission *);
};

Query::Query(const std::string &name, Permission *permission = nullptr) : Resolvers(name, permission) {}

#endif //SG_MCU_QUERY_H
