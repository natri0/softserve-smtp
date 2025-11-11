#ifndef LOGIN_AUTH
#define LOGIN_AUTH
#include "IAuthMethod.h"

class LoginAuth final : public IAuthMethod {
  bool authenticate(const std::string &user, const std::string &secret) override;
};

#endif