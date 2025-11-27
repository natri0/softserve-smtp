#ifndef EMAILMESSAGE_H
#define EMAILMESSAGE_H

#pragma once
#include <string>
#include <vector>
#include <cstdint>


/**
 * @class Attachment
 * @brief Represents a file attachment for an email message.
*/
class Attachment {
private:
    std::string filename;
    std::string mimeType;
    std::string contentDisposition;
    std::vector<uint8_t> data;

public:

    /**
     * @brief Constructs an Attachment object.
     * @param name The file name (cannot be empty).
     * @param type MIME type (default: "application/octet-stream").
     * @param disposition Content disposition (default: "attachment").
     * @param content Binary data for the attachment (default: empty).
     * @throws std::invalid_argument if filename is empty.
    */
    Attachment(const std::string& name,
               const std::string& type = "application/octet-stream",
               const std::string& disposition = "attachment",
               const std::vector<uint8_t>& content = {});

    /// @brief Returns the filename of the attachment.        
    const std::string& getFileName() const { return filename; }

    /// @brief Returns the MIME type of the attachment.
    const std::string& getMimeType() const { return mimeType; }

    /// @brief Returns the content disposition of the attachment.
    const std::string& getContentDisposition() const { return contentDisposition; }
    
    /// @brief Returns the binary content of the attachment.
    const std::vector<uint8_t>& getData() const { return data; }
};

/**
 * @class EmailMessage
 * @brief Represents an email message with headers, recipients, body, and attachments.
 */
class EmailMessage {
private:
    std::string from;
    std::vector<std::string> to;
    std::string subject;
    std::string body;
    std::string contentType;
    std::vector<Attachment> attachments;
public:

    /// @brief Default constructor
    EmailMessage() = default;

    /// @brief Returns the sender email address.
    ~EmailMessage() = default;
    
    /// @brief Returns the sender email address.
    const std::string& getFrom() const { return from; }

    /// @brief Returns the list of recipient email addresses.
    const std::vector<std::string>& getTo() const { return to; }

    /// @brief Returns the email subject.
    const std::string& getSubject() const { return subject; }

    /// @brief Returns the email body.
    const std::string& getBody() const { return body; }

    /// @brief Returns the content type of the email body.
    const std::string& getContentType() const { return contentType; }

    /// @brief Returns the list of attachments.
    const std::vector<Attachment>& getAttachments() const { return attachments; }

    /// @brief Sets the sender email address.
    void setFrom(const std::string& from_) { from = from_; }

    /// @brief Adds a recipient email address.
    void setTo(const std::string& to_) { to.push_back(to_); }

    /// @brief Sets the email subject.
    void setSubject(const std::string& subject_) { subject = subject_; }
    
    /// @brief Sets the email body.
    void setBody(const std::string& body_) { body = body_; }

    /// @brief Sets the content type of the email body.
    void setContentType(const std::string& contentType_) { contentType = contentType_; }
    
    /**
     * @brief Adds an attachment to the email.
     * @param attachment The Attachment object to add.
    */
    void addAttachment(const Attachment& attachment) { 
        attachments.push_back(attachment); 
    }

    /**
     * @brief Adds an attachment to the email by moving it.
     * @param attachment The Attachment object to add (rvalue reference).
    */
    void addAttachment(Attachment&& attachment) { 
        attachments.push_back(std::move(attachment)); 
    }
    
    /// @brief Clears all attachments.
    void clearAttachments() { attachments.clear(); }

    /// @brief Clears all recipients.
    void clearRecipients() { to.clear(); }

    /// @brief Returns the number of attachments.
    size_t getAttachmentCount() const { return attachments.size(); }

    /// @brief Returns true if there is at least one attachment.
    bool hasAttachments() const { return !attachments.empty(); }
};


#endif //EMAILMESSAGE_H
