#ifndef AUTH_METHOD_FACTORY
#define AUTH_METHOD_FACTORY

#include "IAuthMethod.h"
#include "LoginAuth.h"
#include "PlainAuth.h"
#include <memory>

class AuthMethodFactory {
public:
  static std::unique_ptr<IAuthMethod> getLoginMethod();
};

#endif