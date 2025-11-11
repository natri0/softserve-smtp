//
// Created by alkir on 10/20/2025.
//

#ifndef EMAILMESSAGE_H
#define EMAILMESSAGE_H

#include <string>
#include <vector>

struct EmailMessage
{
    std::string from;
    std::vector<std::string> to;
    std::string subject;
    std::string body;
    std::vector<std::string> attachmentPaths;
};

#endif //EMAILMESSAGE_H
