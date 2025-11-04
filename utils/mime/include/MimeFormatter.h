#pragma once
#include "EmailMessage.h"
#include <string>
#include <vector>
#include <cstdint>


class MimeFormatter {
public:

    std::vector<uint8_t> formatBinary(const EmailMessage& message) const;
    
private:

    std::string formatHeaders(const EmailMessage& message, 
                             const std::string& boundary) const;
    

    std::string formatBody(const EmailMessage& message, 
                          const std::string& boundary) const;

    std::vector<uint8_t> formatMultipartBinary(const EmailMessage& message, 
                                               const std::string& boundary) const;

    std::string generateBoundary() const;
};