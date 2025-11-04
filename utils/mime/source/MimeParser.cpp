#include "MimeParser.h"
#include <regex>
#include <sstream>
#include <algorithm>
#include <cctype>

EmailMessage MimeParser::parse(const std::vector<uint8_t>& rawMessage) {
    EmailMessage message;

    std::string data(reinterpret_cast<const char*>(rawMessage.data()), rawMessage.size());

    size_t pos = data.find("\r\n\r\n");
    if (pos == std::string::npos) {
        pos = data.find("\n\n");
        if (pos != std::string::npos) {
            pos += 2;
        }
    } else {
        pos += 4;
    }

    std::string headerSection = data.substr(0, pos < 4 ? 0 : pos - 4);
    std::string bodySection = (pos < data.size()) ? data.substr(pos) : "";

    parseHeaders(headerSection, message);

    std::string boundary = extractBoundaryFromContentType(message.getContentType());
    
    if (!boundary.empty()) {
        parseMultipart(bodySection, boundary, message);
    } else {
        parseBody(bodySection, message);
    }

    return message;
}

bool MimeParser::parseHeaders(const std::string& headerSection, EmailMessage& message) {
    std::istringstream stream(headerSection);
    std::string line;
    std::string currentHeader;

    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        if (line.empty()) continue;

        if (line[0] == ' ' || line[0] == '\t') {
            currentHeader += " " + line;
            continue;
        }

        if (!currentHeader.empty()) {
            processHeader(currentHeader, message);
        }

        currentHeader = line;
    }

    if (!currentHeader.empty()) {
        processHeader(currentHeader, message);
    }

    return true;
}

void MimeParser::processHeader(const std::string& line, EmailMessage& message) {
    auto colonPos = line.find(':');
    if (colonPos == std::string::npos) return;

    std::string key = line.substr(0, colonPos);
    std::string value = line.substr(colonPos + 1);
    trim(value);

    if (key == "From") {
        message.setFrom(value);
    } else if (key == "To") {
        std::stringstream ss(value);
        std::string email;
        while (std::getline(ss, email, ',')) {
            trim(email);
            if (!email.empty()) {
                message.setTo(email);
            }
        }
    } else if (key == "Subject") {
        message.setSubject(value);
    } else if (key == "Content-Type") {
        message.setContentType(value);
    }
}

bool MimeParser::parseBody(const std::string& bodySection, EmailMessage& message) {
    if (bodySection.empty()) {
        message.setBody("");
        return true;
    }

    std::string body = bodySection;
    while (!body.empty() && (body.back() == '\r' || body.back() == '\n')) {
        body.pop_back();
    }
    
    message.setBody(body);
    return true;
}

bool MimeParser::parseMultipart(const std::string& body, const std::string& boundary, EmailMessage& message) {
    std::string boundaryMarker = "--" + boundary;
    std::string endBoundary = "--" + boundary + "--";
    
    size_t pos = 0;

    while (pos < body.size()) {
        size_t partStart = body.find(boundaryMarker, pos);
        if (partStart == std::string::npos) break;

        if (body.compare(partStart, endBoundary.size(), endBoundary) == 0) {
            break;
        }

        partStart += boundaryMarker.size();
        
        while (partStart < body.size() && 
               (body[partStart] == '\r' || body[partStart] == '\n' || 
                body[partStart] == ' ' || body[partStart] == '\t')) {
            partStart++;
        }

        size_t partEnd = body.find("\r\n--" + boundary, partStart);
        if (partEnd == std::string::npos) {
            partEnd = body.find("\n--" + boundary, partStart);
            if (partEnd == std::string::npos) {
                partEnd = body.size();
            }
        }

        std::string part = body.substr(partStart, partEnd - partStart);
        parsePart(part, message);

        pos = partEnd;
    }

    return true;
}

void MimeParser::parsePart(const std::string& part, EmailMessage& message) {
    size_t headerEnd = part.find("\r\n\r\n");
    size_t headerLen = 4;
    
    if (headerEnd == std::string::npos) {
        headerEnd = part.find("\n\n");
        headerLen = 2;
        if (headerEnd == std::string::npos) return;
    }

    std::string headers = part.substr(0, headerEnd);
    std::string content = part.substr(headerEnd + headerLen);

    std::string contentType;
    std::string disposition;

    std::istringstream stream(headers);
    std::string line;
    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty()) continue;

        auto colonPos = line.find(':');
        if (colonPos == std::string::npos) continue;

        std::string key = line.substr(0, colonPos);
        std::string value = line.substr(colonPos + 1);
        trim(value);

        if (key == "Content-Type") {
            contentType = value;
        } else if (key == "Content-Disposition") {
            disposition = value;
        }
    }

    if (disposition.find("attachment") != std::string::npos) {
        std::string filename = extractFilenameFromDisposition(disposition);
        std::vector<uint8_t> fileData(content.begin(), content.end());
        
        message.addAttachment({filename, contentType, "attachment", fileData});
    } else if (contentType.find("text/plain") != std::string::npos || 
               contentType.find("text/html") != std::string::npos) {
        if (message.getBody().empty()) {
            message.setBody(content);
        }
    }
}

std::string MimeParser::extractBoundaryFromContentType(const std::string& contentType) {
    std::regex boundaryRegex(R"(boundary=\"?([^\";]+)\"?)");
    std::smatch match;

    if (std::regex_search(contentType, match, boundaryRegex)) {
        return match[1].str();
    }

    return {};
}

std::string MimeParser::extractFilenameFromDisposition(const std::string& disposition) {
    std::regex fileRegex(R"(filename=\"?([^\";]+)\"?)");
    std::smatch match;

    if (std::regex_search(disposition, match, fileRegex)) {
        return match[1].str();
    }

    return "unknown.bin";
}

void MimeParser::trim(std::string& s) {
    auto notSpace = [](unsigned char ch) { return !std::isspace(ch); };
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), notSpace));
    s.erase(std::find_if(s.rbegin(), s.rend(), notSpace).base(), s.end());
}
