#pragma once
#include "EmailMessage.h"
#include <vector>
#include <string>

class MimeParser {
public:
    EmailMessage parse(const std::vector<uint8_t>& rawMessage);

private:
    bool parseHeaders(const std::string& headerSection, EmailMessage& message);
    void processHeader(const std::string& line, EmailMessage& message);
    bool parseBody(const std::string& bodySection, EmailMessage& message);
    bool parseMultipart(const std::string& body, const std::string& boundary, EmailMessage& message);
    void parsePart(const std::string& part, EmailMessage& message);

    std::string extractBoundaryFromContentType(const std::string& contentType);
    std::string extractFilenameFromDisposition(const std::string& disposition);

    static void trim(std::string& s);
};
