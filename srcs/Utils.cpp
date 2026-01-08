/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaja <yaja@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 12:15:51 by yanaranj          #+#    #+#             */
/*   Updated: 2026/01/08 12:52:12 by yaja             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/* FORMATING RESPONSES*/
//this function will handle to convert all \n into \r\n, so everything will have the IRC format
std::string convertResponse(const std::string &response){
	std::string finalResponse;

	finalResponse.reserve(response.size() + 2);
	for (size_t i = 0; i < response.size(); i++){
		if (response[i] == '\n'){
			if (i == 0 || response[i - 1] !=  '\r')
				finalResponse += '\r';
		}
		finalResponse += response[i];
	}
	if (finalResponse.empty() || finalResponse[finalResponse.size() - 1] != '\n' \
	|| (finalResponse.size() > 1 && finalResponse[finalResponse.size() - 2] != '\r')){
		finalResponse += "\r\n";
	}
	return finalResponse;
}

void Server::sendResponse(int clientFd, const std::string &response){
	std::string formatResponse = convertResponse(response);

	if (send(clientFd, formatResponse.c_str(), formatResponse.size(), 0) == -1){
		std::cerr << RED << "Error sending response to: " << clientFd << " client." << NC << std::endl;
	}
}

/* parsing */
int Utils::check_port(char *port){
    int i = 0;
    long res = 0;

    while (port[i])
    {
        if (!isdigit(static_cast<unsigned char>(port[i])))
            throw std::runtime_error("Error: Invalid port: needs only digits.");
        res = res * 10 + (port[i] - '0');
        i++;
    }
    if (res <= 1023 || res > 65535){
        throw std::runtime_error("Error: Recommended port for IRC: 6667.");
    }
    return (static_cast<int>(res));
}

bool Utils::check_password(const std::string &password){
    if (password.empty() || password.size() > 25)
        return (false);
    for (size_t i = 0; i < password.size(); i++){
        if (password[i] < 33 || password[i] > 126)
            return false;
    }
    return true;
}


//split all the tokens for proper parsing
std::vector<std::string> Utils::split(const std::string &str, char delimiter){
	std::vector<std::string> tokens;
	std::stringstream ss(str);
	std::string token;

	while(std::getline(ss, token, delimiter))
	{
		if (!token.empty() && token[token.size() - 1] == '\r')
    		token.erase(token.size() - 1);
		tokens.push_back(token);
	}
	return tokens;
}


/*various checks*/

bool Server::checkNick(std::string nick)
{
	if (nick.empty())
		return (false);
	int i = 0;
	while (nick[i])
	{
		if (!isalnum(nick[i]) && nick[i] != '_')
			return (false);
		i++;
	}
	return (true);
}
bool Server::checkUser(std::string user) const
{
	if (user.empty())
		return (false);
	int i = 0;
	while (user[i])
	{
		if (!isalnum(user[i]) && user[i] != '_'  && user[i] != '-' && user[i]!= '.')
			return (false);
		i++;
	}
	return (true);
}

bool Server::nickTaken(std::string nick) const
{
	unsigned int i = 0;

	while (i < _clients.size())
	{
		if (_clients[i].getNick() == nick)
		{
			return (true);
		}

		i++;
	}
	return(false);
}

std::string Server::appendToks( const std::vector<std::string> &tokens, int start)
{
	std::string res;

	unsigned int i = start;
	while (i < tokens.size())
	{
		res +=  tokens[i];
		res += ' ';
		i++;
	}
	return (res);
}

bool Server::checkSyn(std::string channel)
{
	if (channel.empty())
		return (false);
	if (channel[0] != '#')
		return (false);
	return(true); 
}

bool Server::channelExist(std::string channel)
{
	//int i = 0;
	std::vector<Channel>::iterator it = _channels.begin();
	while (it != _channels.end())
	{
		if ((*it).getName() == channel)
			return (true);
		it++;
	}
	return (false);
}

void Server::removeChannel(std::string chan)
{
	std::vector<Channel>::iterator it = _channels.begin();

	while (it != _channels.end())
	{
		if (chan == it->getName())
		{
			_channels.erase(it);
			return;
		}
		it++;
	}
}