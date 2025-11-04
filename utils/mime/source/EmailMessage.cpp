#include "EmailMessage.h"
#include <stdexcept>

// Тільки конструктор Attachment (бо має логіку копіювання)
Attachment::Attachment(const std::string& name,
                       const std::string& type,
                       const std::string& disposition,
                       const std::vector<uint8_t>& content)
    : filename(name)
    , mimeType(type)
    , contentDisposition(disposition)
    , data(content.begin(), content.end())  
{
    if (filename.empty()) {
        throw std::invalid_argument("Filename cannot be empty");
    }
}
