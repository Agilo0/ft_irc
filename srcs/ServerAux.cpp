/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerAux.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaja <yaja@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/10 10:48:05 by yanaranj          #+#    #+#             */
/*   Updated: 2025/12/26 20:56:47 by yaja             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Channel *Server::moveCreateChannel(const std::string &channelName){
	//if we have the name on the list, we send the channel
	for(size_t i = 0; i < _channels.size(); i++){
		if (_channels[i].getName() == channelName)
			return &_channels[i];
	}
	//if not, we add this new name on the vector
	_channels.push_back(Channel(channelName));
	return &_channels.back();
}

//this will tell us the client we are talking about (but not its fd)
Client *Server::getClient(int fd){
	for (size_t i = 0; i < _clients.size(); ++i){
		if (_clients[i].getClientFd() == fd)
			return &_clients[i];
	}
	return NULL;
}

Client *Server::getClientByNick(const std::string &dest){
	for (size_t i = 0; i < _clients.size(); ++i){
		if (_clients[i].getNickname() == dest)
			return &_clients[i];
	}
	return NULL;
}

//Notification

void Server::broadcastNewNick(Client *cli)
{
	int i = 0;
	std::vector<int> clientFdsOk;
	std::vector<int> res;
	Channel *chan;

	while ((chan = cli->getChannel(i)) != NULL)
	{
		clientFdsOk = notifChannel(chan, cli->getOldnick(), cli->getNickname(), clientFdsOk);
		i++;
	}
}

std::vector<int> Server::notifChannel(Channel *chan, std::string old, std::string nick, std::vector<int> ok)
{
	std::set<int> clients = chan->getClients();
	std::set<int>::iterator it = clients.begin();
	
	while (it != clients.end())
	{
		int fd = *it;
		if (std::find(ok.begin(), ok.end(), fd) == ok.end())
		{
			sendResponse(fd, NICK_UPDATE(old, nick));
			ok.push_back(fd);
		}
		it++;

	}
	return (ok);
}

void Server::broadcastPart(std::string chann, std::string message, std::string reason)
{

	std::vector<Channel>::iterator it = _channels.begin();

	while (it != _channels.end())
	{
		if (chann == (*it).getName())
		{
			std::set<int> clients = (*it).getClients();
			std::set<int>::iterator itc = clients.begin();
			while (itc != clients.end())
			{
				int fd = *itc;
				sendResponse(fd, RPL_PART(message, (*it).getName(), reason));
				itc++;
			}
			return;
		}
		it++;
	}
}

//I have to handle properly this. I think I should used differents setter for each status??
//When the handShake is finish, I should recieve the RLP_WELCOME!!
void Server::handShake(Client *cli, const std::string &command){
	std::vector<std::string> tokens = Utils::split(command, ' ');
	if (tokens.empty())
		return;
	std::string cmd = tokens[0];
	for(size_t i = 0; i < cmd.size(); ++i)
		cmd[i] = std::toupper(cmd[i]);
	if (cmd == "PASS")
		passAuth(cli, tokens, _serverName);
	else if(cmd == "NICK")
		nickAuth(cli, tokens, _serverName);
	else if (cmd == "USER")
		return ;
	else if (cmd == "JOIN")
		handleJoin(cli, tokens);
	//add rest of existing commands
	else
		std::cout << ORANGE << "WORK IN PROGRESS..." << std::endl;
	if (cli->getStatus() == USER_OK){
		cli->setStatus(AUTHENTICATED);
		std::cout << TURQUOISE << "WELCOME TO IRC SERVER!!!" << NC << std::endl;
		//RPL_WELCOME(cli->getNickname(), _serverName, cli->getClientIP);
	}
}










//JUST FOR DEBUG

//Muestra los canales que hay y que clientes hay en cada uno
void Server::printChannels(const std::vector<Channel>& _channels)
{
    std::cout << "---- Lista de canales ----" << std::endl;
    for (size_t i = 0; i < _channels.size(); ++i)
    {
        const Channel &ch =_channels[i];
        std::cout << "[" << i << "] Nombre: " << ch.getName();

        if (ch.hasTopic())
            std::cout << " | Topic: " << ch.getTopic();

        std::cout << "\n    Clientes (" << ch.getClients().size() << "): ";

        const std::set<int> &clients = ch.getClients();
        std::set<int>::const_iterator it = clients.begin();
        for (; it != clients.end(); ++it)
        {
            std::cout << *it;
            std::set<int>::const_iterator next = it;
            ++next;
            if (next != clients.end())
                std::cout << ", ";
        }
        std::cout << std::endl;
    }
    std::cout << "--------------------------" << std::endl;
}

// Muestra los clientes+fd autenticados y los conectados sin autenticar 
void Server::printClients(const std::vector<Client>& _clients) {
    std::cout << "---- Lista de clientes ----" << std::endl;
    for (size_t i = 0; i < _clients.size(); ++i) {
		if (_clients[i].getStatus() == AUTHENTICATED) {
        	std::cout << "Cliente " << i << " FD=" << _clients[i].getClientFd() 
			<< " " << _clients[i].getNickname() << std::endl;
    	} else {
			std::cout << "Cliente " << i << " FD=" << _clients[i].getClientFd() 
			<< " " << _clients[i].getNickname() << " NOT_AUTHENTICATED" << std::endl;
		}
	}
}