#pragma once
#include "EmailMessage.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>


class MimeFormatter {
public:
    std::vector<uint8_t> formatBinary(const EmailMessage& message) const {
        std::string boundary = generateBoundary();
        
        std::vector<uint8_t> result;
        
        
        std::string header = formatHeaders(message, boundary);
        result.insert(result.end(), header.begin(), header.end());
        
        
        std::string body = formatBody(message, boundary);
        result.insert(result.end(), body.begin(), body.end());
        
        
        std::vector<uint8_t> multiPart = formatMultipartBinary(message, boundary);
        result.insert(result.end(), multiPart.begin(), multiPart.end());
        
        return result;
    }
    
private:
    std::string formatHeaders(const EmailMessage& message, const std::string& boundary) const {
        std::string header = "From: " + message.getFrom() + "\r\n";

        std::vector<std::string> to =  message.getTo();
        header += "To: ";
        for(size_t i = 0; i < to.size(); ++i) {
            header += i != 0? ", " + to[i]: to[i];
        }

        header += "\r\n";
        header += "Subject: " + message.getSubject() + "\r\n";
        header += "MIME-Version: 1.0\r\n";
        header += std::string("Content-Type: multipart/mixed; boundary=\"") + boundary + "\"\r\n";
        header += "\r\n";
        return header; 
    }

    std::string formatBody(const EmailMessage& message, const std::string& boundary) const {
        std::string body = "--" + boundary + "\r\n";
        body += "Content-Type: text/plain; charset=\"utf-8\"\r\n";
        body += "Content-Transfer-Encoding: 7bit\r\n";
        body += "\r\n";
        body += message.getBody() + "\r\n";
        return body;
    }

    std::vector<uint8_t> formatMultipartBinary(const EmailMessage& message, const std::string& boundary) const {
        std::vector<uint8_t> result;
        
        if (message.getAttachments().empty()) {
            std::string end = "--" + boundary + "--\r\n\r\n";
            result.insert(result.end(), end.begin(), end.end());
            return result;
        }
        
        for(const auto& att : message.getAttachments()) {
            // Boundary
            std::string part = "--" + boundary + "\r\n";
            part += "Content-Type: " + att.getMimeType() + "\r\n";
            part += "Content-Disposition: " + att.getContentDisposition() + "; filename=\"" + att.getFileName() + "\"\r\n";
            part += "Content-Transfer-Encoding: binary\r\n";
            part += "\r\n";
            
            result.insert(result.end(), part.begin(), part.end());
            
            // Бінарні дані (можуть містити null байти)
            const auto& data = att.getData();
            result.insert(result.end(), data.begin(), data.end());
            
            // Кінець частини
            std::string lineEnd = "\r\n";
            result.insert(result.end(), lineEnd.begin(), lineEnd.end());
        }
        
        std::string end = "--" + boundary + "--\r\n\r\n";
        result.insert(result.end(), end.begin(), end.end());
        
        return result;
    }
    

    std::string generateBoundary() const {
        boost::uuids::uuid u = boost::uuids::random_generator()();
        return "----=_Boundary_" + boost::uuids::to_string(u);
    }
};