#ifndef IAUTHMETHOD_H
#define IAUTHMETHOD_H

#include <string>

class IAuthMethod {
public:
  virtual ~IAuthMethod() = default;
  virtual bool authenticate(const std::string& user, const std::string& secret) = 0;
};

#endif