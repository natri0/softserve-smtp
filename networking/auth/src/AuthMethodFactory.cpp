#include "../include/AuthMethodFactory.h"
#include <stdexcept>

std::unique_ptr<IAuthMethod> AuthMethodFactory::getLoginMethod() {
  int type = 1;

  // type = config.authMethod.get() or smth like that should be here

  switch (type) {
    case 1: // PLAIN
      return std::make_unique<PlainAuth>();
    case 2:
      return std::make_unique<LoginAuth>();
    default:
      throw std::runtime_error("Invalid authentication method");
  }
}