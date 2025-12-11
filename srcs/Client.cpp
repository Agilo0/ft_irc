#include "Server.hpp"

Client::Client(){}
Client::~Client(){}

void Client::setClientFd(int fd){
	_clientFd = fd;
}
void Client::setUsername(std::string username){
	_username = username;
}
int Client::getClientFd() const{
	return _clientFd;
}

std::string Client::getUsername() const{
	return _username;
}