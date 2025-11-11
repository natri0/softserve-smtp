#ifndef CLIENTSETTINGS_H
#define CLIENTSETTINGS_H

#include <string>

struct ClientSettings{
    std::string server;
    uint16_t port;
    std::string username;
    std::string password;
    std::string logLevel;
    uint16_t securityType = 1;
};


#endif // CLIENTSETTINGS_H
