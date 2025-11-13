#ifndef IAUTHHANDLER_H
#define IAUTHHANDLER_H

#include <string>

namespace ISXSMTP {
  class IAuthHandler {
  public:
    virtual ~IAuthHandler() = default;
    virtual bool Authenticate(std::string& credentials) = 0;
  };
}

#endif