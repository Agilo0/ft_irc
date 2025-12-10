#include "Server.hpp"

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

int Client::getClientFd() const{
	return _clientFd;
}