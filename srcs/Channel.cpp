/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alounici <alounici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/10 10:30:04 by yanaranj          #+#    #+#             */
/*   Updated: 2025/12/30 20:00:28 by alounici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"
#include "../inc/Channel.hpp"

Channel::Channel(const std::string &name) :
	_name(name),_topic(""),_key(""),
	_maxUsers(0), _iMode(false), _tMode(false),
	_kMode(false), _oMode(false), _lMode(false) {}


/*getters*/
std::string Channel::getName() const{
	return _name;
}
std::string Channel::getTopic() const{
	return _topic;
}
std::string Channel::getKey() const{
	return _key;
}
size_t Channel::getMaxUsers() const{
	return _maxUsers;
}
const std::set<int> &Channel::getClients() const{
	return _clients;
}
/*setters*/
void Channel::setTopic(const std::string &topic){
	_topic = topic;
}
void Channel::setKey(const std::string &key){
	_key = key;
	_kMode = true;
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
// bool Channel::hasKey(bool active) {_hasKey = active;}


bool Channel::hasTopic() const {return !_topic.empty();}
bool Channel::isModeI() const {return _iMode; }//this has been previously changed by his set function
bool Channel::isModeT() const {return _tMode; }
bool Channel::isModeK() const {return _kMode; }
bool Channel::isModeO() const {return _oMode; }
bool Channel::isModeL() const {return _lMode; }


///						*WIP									*/
//NOT quite sure if we should use clean or create an specific function
// void Channel::removeClient(int fd){
// 	(void)fd;//but we should use the fd to know the exact client we are talking about
// 	_clients.clear();
// 	_operators.clear();
// }
void Channel::removeInvite(int fd){
	(void)fd;
	//remove the invitation
}
void Channel::removeOperator(int fd){
	(void)fd;
	//remove operator privileges
}

//we add the client to the operators list (if they are on the list)
void Channel::addOperator(int fd){
	if (_clients.count(fd))//it give us the match of the client
		_operators.insert(fd);
}
void Channel::inviteClient(int fd){
	_invited.insert(fd);
}

//new client has been add if we still have space left
bool Channel::addClient(int fd, bool isOperator){
	if (_lMode && _clients.size() >= _maxUsers)
		return false;
	_clients.insert(fd);
	if (isOperator)
		_operators.insert(fd);
	return true;
}
//if we found this fd on the list, means that is a member
bool Channel::isMember(int fd) const{
	return _clients.find(fd) != _clients.end();
}
bool Channel::isOperator(int fd) const{
	return _operators.find(fd) != _operators.end();
}
bool Channel::isInvited(int fd) const{
	return _clients.find(fd) != _clients.end();
}

void Channel::removeClient(int fd)
{
	_clients.erase(fd);
	_operators.erase(fd);
}

bool Channel::isEmpty() const
{
	if (_clients.empty())
		return (true);
	return (false);
}

int Channel::manageModeChange(std::string mode, std::string arg, int targetFd)
{
	bool sign;
	if (mode[0] == '+')
		sign = true;
	else
		sign = false;
	if (mode[1] == 'i')
	{
		setModeI(sign);
		return(0);
	}
	else if (mode[1] == 't')
	{
		setModeT(sign);
		return(0);
	}
	if (arg == "")
	{
		return(1);
	}
	if (mode[1] == 'k')
	{
		manageK(sign, arg);
		return (0);
	}
	if (mode[1] == 'o')
	{
		if (manageO(sign, targetFd) == 1)
			return (2);
	}
	if (mode[1] == 'l')
	{
		if (manageL(sign, arg) == 1)
			return (3);
	}
	return (0);
}

void Channel::manageK(bool sign, std::string arg)
{
	setModeK(sign);
	if (sign == true)
	{
		_key = arg;
	}
	else
		_key.clear();

}

int Channel::manageO(bool sign, int targetFd)
{
	if (!isMember(targetFd))
		return (1);
	if (sign == true)
		addOperator(targetFd);
	else if (sign == false)
		removeOperator(targetFd);
	setModeO(sign);
	return (0);
}

int Channel::manageL(bool sign, std::string arg)
{
	if (sign == true)
	{
		if (!isStrictNumber(arg))
        	return (1); 

		size_t limit;
		std::stringstream ss(arg);
		ss >> limit;

		if (limit == 0)
			return (1);
		_maxUsers = limit;
		setModeL(true);
	}
	else
		setModeL(sign);
	return (0);
}

bool Channel::isStrictNumber(const std::string &s)
{
    size_t i = 0;

    if (s.empty())
        return false;

    while (i < s.size())
    {
        if (!std::isdigit(s[i]))
            return false;
        i++;
    }
    return true;
}
