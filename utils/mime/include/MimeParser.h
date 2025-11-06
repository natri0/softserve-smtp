#pragma once
#include "EmailMessage.h"
#include <vector>
#include <string>

/**
 * @class MimeParser
 * @brief Parses raw MIME email data into an EmailMessage object.
 */
class MimeParser {
public:
    /**
     * @brief Parses raw email message data into an EmailMessage object.
     * @param rawMessage A vector of bytes representing the raw email data.
     * @return A populated EmailMessage object.
    */
    EmailMessage parse(const std::vector<uint8_t>& rawMessage);

private:
    /**
     * @brief Parses the header section of the email and populates the EmailMessage.
     * @param headerSection The headers as a string.
     * @param message The EmailMessage object to populate.
     * @return True if parsing was successful.
    */
    bool parseHeaders(const std::string& headerSection, EmailMessage& message);
    
    /**
     * @brief Processes a single header line and updates the EmailMessage accordingly.
     * @param line A single header line.
     * @param message The EmailMessage object to update.
    */
    void processHeader(const std::string& line, EmailMessage& message);
    
    /**
     * @brief Parses the body of a simple (non-multipart) email.
     * @param bodySection The body as a string.
     * @param message The EmailMessage object to populate.
     * @return True if parsing was successful.
    */
    bool parseBody(const std::string& bodySection, EmailMessage& message);
    
    /**
     * @brief Parses a multipart body and extracts individual parts.
     * @param body The full body of the email.
     * @param boundary The MIME boundary string.
     * @param message The EmailMessage object to populate.
     * @return True if parsing was successful.
    */
    bool parseMultipart(const std::string& body, const std::string& boundary, EmailMessage& message);
    
    /**
     * @brief Parses a single part of a multipart message.
     * @param part The raw part string.
     * @param message The EmailMessage object to update.
    */
    void parsePart(const std::string& part, EmailMessage& message);

    /**
     * @brief Extracts the MIME boundary from a Content-Type header.
     * @param contentType The value of the Content-Type header.
     * @return The boundary string if found, otherwise an empty string.
    */
    std::string extractBoundaryFromContentType(const std::string& contentType);
    
    /**
     * @brief Extracts the filename from a Content-Disposition header.
     * @param disposition The value of the Content-Disposition header.
     * @return The filename, or "unknown.bin" if not found.
    */
    std::string extractFilenameFromDisposition(const std::string& disposition);

    /**
     * @brief Trims leading and trailing whitespace from a string.
     * @param s The string to trim.
    */
    static void trim(std::string& s);
};
