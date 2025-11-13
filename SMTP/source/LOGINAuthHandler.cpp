#include "../include/LOGINAuthHandler.h"
#include "../../utils/Base64.h"
#include <stdexcept>

bool ISXSMTP::LOGINAuthHandler::Authenticate(std::string& credentials) {
  if (credentials.empty())
    return false;

  try {
    const auto decoded = Base64::Decode({credentials.begin(), credentials.end()});
    return verifyCredentials(std::string(decoded.begin(), decoded.end()));
  } catch (...) {
    return false;
  }
}

bool ISXSMTP::LOGINAuthHandler::verifyCredentials(const std::string& credentials) {
  // TODO: Replace with actual database lookup
  if (username.empty()) {
    username = credentials;
    return false;
  }

  return username == "testuser" && credentials == "testpass";
}

std::string ISXSMTP::LOGINAuthHandler::GetName() const {
  return "LOGIN";
}
