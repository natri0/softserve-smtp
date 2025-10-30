#pragma once
#include "EmailMessage.h"
#include "MimeFormatter.h"

class EmailBuilder {
private:
    EmailMessage message;
    
public:
    EmailBuilder& from(const std::string& from) {

        message.setFrom(from);
        return *this;
    }
    
    EmailBuilder& to(const std::string& to) {

        message.setTo(to);
        return *this;
    }
    
    EmailBuilder& subject(const std::string& subject) {

        message.setSubject(subject);
        return *this;
    }
    
    EmailBuilder& body(const std::string& body, const std::string& contentType = "text/plain") {

        message.setBody(body);
        message.setContentType(contentType);
        return *this;
    }
    
    EmailBuilder& attachment(const std::string& filename, const std::string& type = "application/octet-stream", 
            const std::string& disposition = "attachment", const std::vector<uint8_t> data = {}) {

        message.addAttachment({filename, type, disposition, std::move(data)});
        return *this;
    }
    
    std::vector<uint8_t> buildBinary() const {
        MimeFormatter formatter;
        return formatter.formatBinary(message);
    }
    
    const EmailMessage& getMessage() const { return message; }
};