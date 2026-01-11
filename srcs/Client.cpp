/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yanaranj <yanaranj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 14:36:25 by yanaranj          #+#    #+#             */
/*   Updated: 2026/01/11 17:12:57 by yanaranj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Client::Client(){
    hasNick = false;
	hasUser = false;
	hasPass = false;
	_status = NOT_AUTHENTICATED;
    _toRemove = false;
}

Client::~Client() {}

//setters
void Client::setStatus(Status status) { _status = status; }
void Client::setClientFd(int fd) { _clientFd = fd; }
void Client::setClientIP(const std::string &newIP) { _ip = newIP; }
void Client::setPass() { hasPass = true; }
void Client::setFirstNick(std::string nick){
	_nickname = nick;
	hasNick = true;
}
void Client::setNewNick(std::string nick){
	_oldnick = _nickname;
	_nickname = nick;
}
void Client::setUser(std::string name){
	_username = name;
	hasUser = true;
}
void Client::setRealName(std::string name) { _realname = name; }
void Client::setChannel(Channel *channel) { _channels.push_back(channel->getName()); }

//getters
Status Client::getStatus() { return _status; }
int Client::getClientFd() const { return _clientFd; }
std::string Client::getClientIP() const { return _ip; }
std::string Client::getUsername() const { return _username; }
std::string Client::getNickname() const { return _nickname;}
std::string &Client::getBuff() { return _buffer; }
std::string Client::getNick() const { return (_nickname); }
std::string Client::getOldnick() const { return (_oldnick); }
std::string Client::getRealname() const { return (_realname); }

//bools
bool Client::hasAll() const{
	if (hasPass == true && hasNick == true && hasUser == true)
		return (true);
	return (false);
}
bool Client::hasNickname() const { return (hasNick);}
bool Client::hasUsername() const { return (hasUser); }
bool Client::hasPassw() const { return (hasPass); }
bool Client::isToRemove() const { return (_toRemove); }

//others
void Client::markForRevome() { _toRemove = true; }
void Client::addBuffer(const std::string &data){ _buffer += data; }
std::string Client::createMessage(){
	std::string res;

	res.append(":");
	res.append(_nickname);
	res.append("!");
	res.append(_username);
	res.append("@");
	res.append(_ip);
	res.append(" ");
	return (res);
}
std::vector<std::string> Client::getChannelVect() { return (_channels); }
