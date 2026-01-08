/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerAux.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alounici <alounici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/10 10:48:05 by yanaranj          #+#    #+#             */
/*   Updated: 2026/01/08 20:04:51 by alounici         ###   ########.fr       */
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

//I have to handle properly this. I think I should used differents setter for each status??
//When the handShake is finish, I should recieve the RLP_WELCOME!!
// void Server::handShake(Client *cli, const std::string &command){
// 	std::vector<std::string> tokens = Utils::split(command, ' ');
// 	if (tokens.empty())
// 		return;
// 	std::string cmd = tokens[0];
// 	for(size_t i = 0; i < cmd.size(); ++i)
// 		cmd[i] = std::toupper(cmd[i]);
// 	if (cmd == "PASS")
// 		passAuth(cli, tokens);
// 	else if(cmd == "NICK")
// 		nickAuth(cli, tokens);
// 	else if (cmd == "USER")
// 		userAuth(cli, tokens);
// 	else if (cmd == "JOIN")
// 		handleJoin(cli, tokens);
// 	else if (cmd == "KICK")
// 		handleKick(cli, tokens);
// 	else if (cmd == "MODE")
// 		handleMode(cli, tokens);
// 	else if (cmd == "QUIT")
// 		handleQuit(cli, tokens);
// 	else if (cmd == "TOPIC")
// 		handleTopic(cli, tokens);
// 	//add rest of existing commands
// 	else
// 		std::cout << ORANGE << "WORK IN PROGRESS..." << std::endl;
// 	if (cli->getStatus() == USER_OK){
// 		cli->setStatus(AUTHENTICATED);
// 		std::cout << TURQUOISE << "WELCOME TO IRC SERVER!!!" << NC << std::endl;
// 		//RPL_WELCOME(cli->getNickname(), _serverName, cli->getClientIP);
// 	}
// }


void Server::handShake(Client *cli, const std::string &command){
	std::vector<std::string> tokens = Utils::split(command, ' ');
	if (tokens.empty())
		return;
	std::string cmd = tokens[0];
	for(size_t i = 0; i < cmd.size(); ++i)
		cmd[i] = std::toupper(cmd[i]);
	if (cmd == "CAP")
		std::cout << "IS LS\n";
	else if (cmd == "PASS")
		passAuth(cli, tokens);
	else if(cmd == "NICK")
		nickAuth(cli, tokens);
	else if (cmd == "USER")
		userAuth(cli, tokens);
/* 	else if (cmd == "JOIN"){
		std::cout << "handshake join\n";
		handleJoin(cli, tokens);
	} */
	else
	//check this error, because protocol has no params!!!!!
		sendResponse(cli->getClientFd(), ERR_NOTREGISTERED((cli->getNickname())));
	if (cli->hasAll()){
		cli->setStatus(AUTHENTICATED);
		std::cout << YELLOW << "<" << cli->getClientFd() << "> " << "Authenticated: "
			<< cli->getNickname() << NC << std::endl;
		sendResponse(cli->getClientFd(), RPL_WELCOME(cli->getNickname(), _serverName, cli->getClientIP()));
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