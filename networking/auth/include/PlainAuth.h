#ifndef PLAIN_AUTH
#define PLAIN_AUTH
#include "IAuthMethod.h"

class PlainAuth final : public IAuthMethod {
  bool authenticate(const std::string &user, const std::string &secret) override;
};

#endif