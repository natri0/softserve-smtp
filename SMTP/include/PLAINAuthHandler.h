#ifndef PLAIN_AUTH_HANDLER_H
#define PLAIN_AUTH_HANDLER_H

#include "IAuthHandler.h"
#include <string>
#include <vector>

namespace ISXSMTP {
  class PLAINAuthHandler final : public IAuthHandler {
  public:
    bool Authenticate(std::string& credentials) override;

  private:
    static std::pair<std::string, std::string> parseCredentials(const std::vector<unsigned char>& decoded);
    static bool verifyCredentials(const std::string& username, const std::string& password);
  };
}

#endif