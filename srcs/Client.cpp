#include "../inc/Client.hpp"

Client::Client()
{
}
Client::Client(int fd) : _clientFd(fd)
{
    (void)_clientFd;
}

Client::~Client()
{
}