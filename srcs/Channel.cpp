/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yanaranj <yanaranj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/10 10:30:04 by yanaranj          #+#    #+#             */
/*   Updated: 2025/12/10 12:53:50 by yanaranj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Channel::Channel(const std::string &name) :
	_name(name),_topic(""),_key(""),
	_maxUsers(0), _iMode(false), _tMode(false),
	_kMode(false), _oMode(false), _lMode(false) {}

std::string Channel::getName() const{
	return _name;
}
std::string Channel::getTopic() const{
	return _topic;
}
std::string Channel::getKey() const{
	return _key;
}
int Channel::getMaxUsers() const{
	return _maxUsers;
}
const std::set<int> &Channel::getClients() const{
	return _clients;
}

void Channel::setTopic(const std::string &topic){
	_topic = topic;
}
void Channel::setKey(const std::string &key){
	_key = key;
}
void Channel::setMaxUsers(int max){
	_maxUsers = max;
}


/*MODES*/

void Channel::setModeI(bool active){ _iMode = active;}
void Channel::setModeT(bool active){ _tMode = active;}
void Channel::setModeK(bool active){ _kMode = active;}
void Channel::setModeO(bool active){ _oMode = active;}
void Channel::setModeL(bool active){ _lMode = active;}


bool Channel::hasTopic() const {return !_topic.empty();}
bool Channel::isModeI() const {return _iMode; }//this has been previously changed by his set function
bool Channel::isModeT() const {return _tMode; }
bool Channel::isModeK() const {return _kMode; }
bool Channel::isModeO() const {return _oMode; }
bool Channel::isModeL() const {return _lMode; }


//declare the rest of the functions
//NOT quite sure if we should use clean or create an specific function
void Channel::removeClient(int fd){
	(void)fd;//but we should use the fd to know the exact client we are talking about
	_clients.clear();
	_operators.clear();
}


