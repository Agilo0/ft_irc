#include "server.hpp"
#include <iostream>
#include <sys/socket.h>



server::server()
{

}

server::server(int port, char *pw) : _port(port), _pw(pw)
{
    init_serv();
}

server::~server()
{

}

void server::init_serv()
{
    _fd = socket(AF_INET, SOCK_STREAM, 0);
}