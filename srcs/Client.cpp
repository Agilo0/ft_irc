#include "../inc/Server.hpp"
#include "../inc/Client.hpp"

Client::Client(int fd)
{ 
	_clientFd = fd; 
	hasNick = false;
	hasUser = false;
	hasPass = false;
	logged = false;
}

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



bool Client::isLogged() const
{
	if (logged == true)
		return (true);
	return (false);
}

void Client::setPass() 
{
	hasPass = true;
}

bool Client::hasAll() const
{
	if (hasPass == true && hasNick == true && hasUser == true)
		return (true);
	return (false);
}

bool Client::hasNickname() const
{
	return (hasNick);
}

bool Client::hasUsername() const
{
	return (hasUser);
}

bool Client::hasPassw() const
{
	return (hasPass);
}

void Client::setFirstNick(std::string nick)
{
	_nickname = nick;
	hasNick = true;
}

void Client::setNewNick(std::string nick)
{
	_oldnick = _nickname;
	_nickname = nick;
}


void Client::setLog()
{
	logged = true;
}

std::string Client::getNick() const
{
	return (_nickname);
}

std::string Client::getOldnick() const
{
	return (_oldnick);
}

void Client::setChannel(Channel &channel)
{
	_channels.push_back(channel);
}


Channel *Client::getChannel(unsigned int index)
{
	if (_channels.size() > index)
		return (&_channels[index]);
	return (NULL);
}

void Client::addBuffer(const std::string &data){
	_buffer += data;
}