#include "Server.hpp"

Client::Client(int fd){ _clientFd = fd; }
Client::Client(){}
Client::~Client(){}

void Client::setClientFd(int fd){
	_clientFd = fd;
}
void Client::setClientIP(const std::string &newIP) { _ip = newIP; }
void Client::setClientInfo(std::string username, std::string nickname){
	_username = username;
	_nickname = nickname;
}
int Client::getClientFd() const{
	return _clientFd;
}

std::string Client::getUsername() const{
	return _username;
}
std::string Client::getNickname() const{
	return _nickname;
}
std::string Client::getClientIP() const{
	return _ip;
}
std::string &Client::getBuff(){
	return _buffer;
}

void Client::addBuffer(const std::string &data){
	_buffer += data;
}