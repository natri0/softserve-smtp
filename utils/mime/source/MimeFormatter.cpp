#include "MimeFormatter.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <stdexcept>

// ============================================================================
// Public Methods
// ============================================================================

std::vector<uint8_t> MimeFormatter::formatBinary(const EmailMessage& message) const {
    std::string boundary = generateBoundary();
    std::vector<uint8_t> result;
    
    // Format headers
    std::string header = formatHeaders(message, boundary);
    result.insert(result.end(), header.begin(), header.end());
    
    // Format body
    std::string body = formatBody(message, boundary);
    result.insert(result.end(), body.begin(), body.end());
    
    // Format attachments
    std::vector<uint8_t> multiPart = formatMultipartBinary(message, boundary);
    result.insert(result.end(), multiPart.begin(), multiPart.end());
    
    return result;
}

// ============================================================================
// Private Methods
// ============================================================================

std::string MimeFormatter::formatHeaders(const EmailMessage& message, 
                                        const std::string& boundary) const {
    std::string header;
    header.reserve(512); // Pre-allocate для оптимізації
    
    // From
    header += "From: " + message.getFrom() + "\r\n";
    
    // To (comma-separated)
    const std::vector<std::string>& to = message.getTo();
    if (!to.empty()) {
        header += "To: ";
        for (size_t i = 0; i < to.size(); ++i) {
            if (i != 0) header += ", ";
            header += to[i];
        }
        header += "\r\n";
    }
    
    // Subject
    header += "Subject: " + message.getSubject() + "\r\n";
    
    // MIME headers
    header += "MIME-Version: 1.0\r\n";
    header += "Content-Type: multipart/mixed; boundary=\"" + boundary + "\"\r\n";
    header += "\r\n";
    
    return header;
}

std::string MimeFormatter::formatBody(const EmailMessage& message, 
                                     const std::string& boundary) const {
    std::string body;
    body.reserve(message.getBody().size() + 256);
    
    // Body part boundary
    body += "--" + boundary + "\r\n";
    body += "Content-Type: text/plain; charset=\"utf-8\"\r\n";
    body += "Content-Transfer-Encoding: 7bit\r\n";
    body += "\r\n";
    
    // Actual body content
    body += message.getBody();
    body += "\r\n";
    
    return body;
}

std::vector<uint8_t> MimeFormatter::formatMultipartBinary(
    const EmailMessage& message, 
    const std::string& boundary) const {
    
    std::vector<uint8_t> result;
    
    // If no attachments, just close boundary
    if (message.getAttachments().empty()) {
        std::string end = "--" + boundary + "--\r\n\r\n";
        result.insert(result.end(), end.begin(), end.end());
        return result;
    }
    
    // Process each attachment
    for (const auto& att : message.getAttachments()) {
        // Attachment part boundary
        std::string part = "--" + boundary + "\r\n";
        part += "Content-Type: " + att.getMimeType() + "\r\n";
        part += "Content-Disposition: " + att.getContentDisposition() + 
                "; filename=\"" + att.getFileName() + "\"\r\n";
        part += "Content-Transfer-Encoding: binary\r\n";
        part += "\r\n";
        
        result.insert(result.end(), part.begin(), part.end());
        
        // Binary data
        const auto& data = att.getData();
        result.insert(result.end(), data.begin(), data.end());
        
        // Line ending
        std::string lineEnd = "\r\n";
        result.insert(result.end(), lineEnd.begin(), lineEnd.end());
    }
    
    // Final boundary
    std::string end = "--" + boundary + "--\r\n\r\n";
    result.insert(result.end(), end.begin(), end.end());
    
    return result;
}

std::string MimeFormatter::generateBoundary() const {
    boost::uuids::uuid u = boost::uuids::random_generator()();
    return "----=_Boundary_" + boost::uuids::to_string(u);
}