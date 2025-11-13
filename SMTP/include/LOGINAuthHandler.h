#ifndef LOGINAUTHHANDLER_H
#define LOGINAUTHHANDLER_H

#include "IAuthHandler.h"

namespace ISXSMTP {
  class LOGINAuthHandler final : public IAuthHandler {
  public:
    bool Authenticate(std::string &credentials) override;
    std::string GetName() const override;

  private:
    std::string username;
    bool verifyCredentials(const std::string& credentials);
  };
}
#endif