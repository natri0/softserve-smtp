#include <algorithm>
#include <stdexcept>
#include <iostream>

#include "../../utils/Base64.h"
#include "PLAINAuthHandler.h"

bool ISXSMTP::PLAINAuthHandler::Authenticate(std::string& credentials) {
    if (credentials.empty())
        return false;

    try {
        const auto decoded = Base64::Decode({credentials.begin(), credentials.end()});
        auto [username, password] = parseCredentials(decoded);
        return verifyCredentials(username, password);
    } catch (...) {
        return false;
    }
}

std::pair<std::string, std::string> ISXSMTP::PLAINAuthHandler::parseCredentials(const std::vector<unsigned char>& decoded) {
    std::string username, password;
    size_t pos = 1;

    // Extract username
    while (pos < decoded.size() && decoded[pos] != '\0')
        username += static_cast<char>(decoded[pos++]);

    if (pos >= decoded.size())
        throw std::runtime_error("Invalid PLAIN format");

    pos++;

    // Extract password
    while (pos < decoded.size())
        password += static_cast<char>(decoded[pos++]);

    return {username, password};
}

bool ISXSMTP::PLAINAuthHandler::verifyCredentials(const std::string& username, const std::string& password) {
    // TODO: Replace with actual database lookup
    return username == "testuser" && password == "testpass";
}