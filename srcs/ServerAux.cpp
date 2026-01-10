/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerAux.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alounici <alounici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 14:36:25 by yanaranj          #+#    #+#             */
/*   Updated: 2026/01/10 22:11:08 by alounici         ###   ########.fr       */
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

//We want the access just to that client
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

void Server::close_fds(std::vector<pollfd> &pollFds)
{
	int i = pollFds.size() - 1;
	
	while (i >= 0){
		close(pollFds[i].fd);
		i--;
	}
	close(_servFd);
}

void Server::clearClient(int fd){
	Client *cli = getClient(fd);
	if (!cli)
		return ;
	cli->markForRevome();
	shutdown(fd, SHUT_RDWR);
	close(fd);
	std::cout << PURPLE << "<" << fd << "> Disconnected!" << NC << std::endl;
}




//DELETE LATEEER!!!!!!!!!!!
void Server::printChannels(const std::vector<Channel>& _channels){
    std::cout << "---- Lista de canales ----" << std::endl;
    for (size_t i = 0; i < _channels.size(); ++i){
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
void Server::printClients(std::vector<Client>& _clients) {
    std::cout << "---- Lista de clientes ----" << std::endl;
    for (size_t i = 0; i < _clients.size(); ++i) {
		if (_clients[i].getStatus() == AUTHENTICATED){
        	std::cout << "Cliente " << i << " FD=" << _clients[i].getClientFd() 
			<< " " << _clients[i].getNickname() << std::endl;
    	} else {
			std::cout << "Cliente " << i << " FD=" << _clients[i].getClientFd() 
			<< " " << _clients[i].getNickname() << " NOT_AUTHENTICATED" << std::endl;
		}
	}
}