#pragma once
#include "EmailMessage.h"
#include "MimeFormatter.h"
#include <string>
#include <vector>
#include <cstdint>

class EmailBuilder {
private:
    EmailMessage message;
    
public:
    // Fluent API methods
    EmailBuilder& from(const std::string& from);
    EmailBuilder& to(const std::string& to);
    EmailBuilder& subject(const std::string& subject);
    EmailBuilder& body(const std::string& body, const std::string& contentType = "text/plain");
    EmailBuilder& attachment(const std::string& filename, 
                            const std::string& type = "application/octet-stream",
                            const std::string& disposition = "attachment",
                            const std::vector<uint8_t> data = {});
    
    // Build final MIME message
    std::vector<uint8_t> buildBinary() const;
    
    // Accessor
    const EmailMessage& getMessage() const;
};