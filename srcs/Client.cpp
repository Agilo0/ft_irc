/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaja <yaja@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 14:36:25 by yanaranj          #+#    #+#             */
/*   Updated: 2026/01/01 20:12:54 by yaja             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(){
    hasNick = false;
	hasUser = false;
	hasPass = false;
	logged = false;
	_status = NOT_AUTHENTICATED;
    _toRemove = false;
}

Client::~Client() {}

//setters
void Client::setStatus(Status status){ _status = status; }
void Client::setClientFd(int fd){ _clientFd = fd; }
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
void Client::setLog(){ logged = true; }
void Client::setUser(std::string name){
	_username = name;
	hasUser = true;
}
void Client::setRealName(std::string name) { _realname = name; }
//void Client::setChannel(Channel *channel) { _channels.push_back(channel); }



//getters
Status Client::getStatus() { return _status; }
int Client::getClientFd() const { return _clientFd; }
std::string Client::getClientIP() const { return _ip; }
std::string Client::getUsername() const { return _username; }
std::string Client::getNickname() const { return _nickname;}
std::string &Client::getBuff() { return _buffer; }
std::string Client::getNick() const { return (_nickname); }
std::string Client::getOldnick() const{ return (_oldnick); }
/* Channel *Client::getChannel(unsigned int index){
	if (_channels.size() > index)
		return (_channels[index]);
	return (NULL);
} */


//bools
bool Client::isLogged() const{
	if (logged == true)
		return (true);
	return (false);
}
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