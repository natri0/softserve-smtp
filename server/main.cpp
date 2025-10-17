#include "Server.h"

int main()
{
    std::shared_ptr<Server> server = std::make_shared<Server>();

    server->init();
    server->run();

    return 0;
}
