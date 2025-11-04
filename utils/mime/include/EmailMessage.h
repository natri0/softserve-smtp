#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <map>

class Attachment {
private:
    std::string filename;
    std::string mimeType;
    std::string contentDisposition;
    std::vector<uint8_t> data;

public:

    Attachment(const std::string& name,
               const std::string& type = "application/octet-stream",
               const std::string& disposition = "attachment",
               const std::vector<uint8_t>& content = {});

    const std::string& getFileName() const { return filename; }
    const std::string& getMimeType() const { return mimeType; }
    const std::string& getContentDisposition() const { return contentDisposition; }
    const std::vector<uint8_t>& getData() const { return data; }
};

class EmailMessage {
private:
    std::string from;
    std::vector<std::string> to;
    std::string subject;
    std::string body;
    std::string contentType;
    std::vector<Attachment> attachments;
    std::map<std::string, std::string> headers;

public:
    // Default constructor/destructor
    EmailMessage() = default;
    ~EmailMessage() = default;
    
    EmailMessage(const EmailMessage&) = delete;
    EmailMessage(EmailMessage&&) noexcept = delete;
    EmailMessage& operator=(const EmailMessage&) = delete;
    EmailMessage& operator=(EmailMessage&&) noexcept = delete;

    const std::string& getFrom() const { return from; }
    const std::vector<std::string>& getTo() const { return to; }
    const std::string& getSubject() const { return subject; }
    const std::string& getBody() const { return body; }
    const std::string& getContentType() const { return contentType; }
    const std::vector<Attachment>& getAttachments() const { return attachments; }
    const std::map<std::string, std::string>& getHeaders() const { return headers; }

    void setFrom(const std::string& from_) { from = from_; }
    void setTo(const std::string& to_) { to.push_back(to_); }
    void setSubject(const std::string& subject_) { subject = subject_; }
    void setBody(const std::string& body_) { body = body_; }
    void setContentType(const std::string& contentType_) { contentType = contentType_; }
    
    void addAttachment(const Attachment& attachment) { 
        attachments.push_back(attachment); 
    }
    void addAttachment(Attachment&& attachment) { 
        attachments.push_back(std::move(attachment)); 
    }
    
    void clearAttachments() { attachments.clear(); }
    void clearRecipients() { to.clear(); }
    size_t getAttachmentCount() const { return attachments.size(); }
    bool hasAttachments() const { return !attachments.empty(); }
};