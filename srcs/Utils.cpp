/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alounici <alounici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 12:15:51 by yanaranj          #+#    #+#             */
/*   Updated: 2025/12/23 21:36:39 by alounici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"


/*	FORMATING RESPONSES	*/
//this function will handle to convert all \n into \r\n, so everything will have the IRC format
std::string convertResponse(const std::string &response){
	std::string finalResponse;

	finalResponse.reserve(response.size() + 2); //stores the spaces that we need
	for (size_t i = 0; i < response.size(); i++){
		if (response[i] == '\n'){
			if (i == 0 || response[i - 1] !=  '\r')
				finalResponse += '\r';
		}
		finalResponse += response[i];//add the current char to the final string
	}
	if (finalResponse.empty() || finalResponse[finalResponse.size() - 1] != '\n' \
	|| (finalResponse.size() > 1 && finalResponse[finalResponse.size() - 2] != '\r')){
		finalResponse += "\r\n";
	}
	return finalResponse;
}

//this function makes sure that we are sending the proper response
void Server::sendResponse(int clientFd, const std::string &response){
	std::string formatResponse = convertResponse(response);

	if (send(clientFd, formatResponse.c_str(), formatResponse.size(), 0) == -1){
		std::cerr << RED << "Error sending response to: " << clientFd << " client." << NC << std::endl;
	}
}

//split all the tokens for proper parsing
std::vector<std::string> Utils::split(const std::string &str, char delimiter){
	std::vector<std::string> tokens;
	std::stringstream ss(str);
	std::string token;

	while(std::getline(ss, token, delimiter))
		tokens.push_back(token);
	return tokens;
}

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

std::string Server::getServerName()
{
	return(_serverName);
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
	int i = 0;
	std::vector<Channel>::iterator it = _channels.begin();
	while (it != _channels.end())
	{
		if ((*it).getName() == channel)
			return (true);
		i++;
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
