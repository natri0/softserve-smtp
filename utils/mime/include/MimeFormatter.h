#pragma once
#include "EmailMessage.h"
#include <string>
#include <vector>
#include <cstdint>

/**
 * @class MimeFormatter
 * @brief Responsible for formatting an EmailMessage into a valid MIME binary representation.
*/
class MimeFormatter {
public:
    
    /**
     * @brief Formats the entire email message (headers, body, attachments) into a binary vector.
     * @param message The email message to format.
     * @return A vector of bytes representing the complete MIME message.
    */
    std::vector<uint8_t> formatBinary(const EmailMessage& message) const;
    
private:

    /**
     * @brief Formats the headers of the email message.
     * @param message The email message whose headers to format.
     * @param boundary The boundary string for separating MIME parts.
     * @return Formatted headers as a string.
    */
    std::string formatHeaders(const EmailMessage& message, 
                             const std::string& boundary) const;
    
    /**
     * @brief Formats the body part of the email message.
     * @param message The email message whose body to format.
     * @param boundary The boundary string for separating MIME parts.
     * @return Formatted body as a string.
    */
    std::string formatBody(const EmailMessage& message, 
                          const std::string& boundary) const;

    /**
     * @brief Formats all attachments of the email message into MIME binary parts.
     * @param message The email message whose attachments to format.
     * @param boundary The boundary string for separating MIME parts.
     * @return A vector of bytes representing all attachments.
    */
    std::vector<uint8_t> formatMultipartBinary(const EmailMessage& message, 
                                               const std::string& boundary) const;

    /**
     * @brief Generates a unique MIME boundary string.
     * @return A string that can be used as a MIME boundary.
    */
    std::string generateBoundary() const;
};