#pragma once
#include "EmailMessage.h"
#include "MimeFormatter.h"
#include <string>
#include <vector>
#include <cstdint>

/**
 * @class EmailBuilder
 * @brief Fluent API for building email messages in MIME format.
 * 
 * Allows step-by-step addition of sender, recipient, subject, body, and attachments.
*/
class EmailBuilder {
private:
    EmailMessage message;
    
public:

    /**
     * @brief Sets the sender address.
     * @param from The sender's email address as a string.
     * @return Reference to the current EmailBuilder object for method chaining.
    */
    EmailBuilder& from(const std::string& from);

    /**
     * @brief Sets the recipient address.
     * @param to The recipient's email address as a string.
     * @return Reference to the current EmailBuilder object for method chaining.
    */
    EmailBuilder& to(const std::string& to);

    /**
     * @brief Sets the subject of the email.
     * @param subject The subject string.
     * @return Reference to the current EmailBuilder object for method chaining.
    */
    EmailBuilder& subject(const std::string& subject);

    /**
     * @brief Sets the email body and its content type.
     * @param body The body text.
     * @param contentType The MIME content type (default: "text/plain").
     * @return Reference to the current EmailBuilder object for method chaining.
    */
    EmailBuilder& body(const std::string& body, const std::string& contentType = "text/plain");
    
    /**
     * @brief Adds an attachment to the email.
     * @param filename Name of the attachment file.
     * @param type MIME type of the file (default: "application/octet-stream").
     * @param disposition Attachment disposition (default: "attachment").
     * @param data Binary data of the attachment.
     * @return Reference to the current EmailBuilder object for method chaining.
    */
    EmailBuilder& attachment(const std::string& filename, 
                            const std::string& type = "application/octet-stream",
                            const std::string& disposition = "attachment",
                            const std::vector<uint8_t> data = {});
    
    /**
     * @brief Builds the final MIME message as a binary array.
     * @return A vector of bytes representing the MIME message.
    */
    std::vector<uint8_t> buildBinary() const;
    
    /**
     * @brief Gets the internal EmailMessage object.
     * @return Constant reference to the internal EmailMessage.
    */
    const EmailMessage& getMessage() const;
};