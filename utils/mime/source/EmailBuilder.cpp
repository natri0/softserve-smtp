#include "EmailBuilder.h"

EmailBuilder& EmailBuilder::from(const std::string& from) {
    message.setFrom(from);
    return *this;
}

EmailBuilder& EmailBuilder::to(const std::string& to) {
    message.setTo(to);
    return *this;
}

EmailBuilder& EmailBuilder::subject(const std::string& subject) {
    message.setSubject(subject);
    return *this;
}

EmailBuilder& EmailBuilder::body(const std::string& body, const std::string& contentType) {
    message.setBody(body);
    message.setContentType(contentType);
    return *this;
}

EmailBuilder& EmailBuilder::attachment(const std::string& filename,
                                      const std::string& type,
                                      const std::string& disposition,
                                      const std::vector<uint8_t> data) {
    message.addAttachment({filename, type, disposition, std::move(data)});
    return *this;
}

std::vector<uint8_t> EmailBuilder::buildBinary() const {
    MimeFormatter formatter;
    return formatter.formatBinary(message);
}

const EmailMessage& EmailBuilder::getMessage() const {
    return message;
}