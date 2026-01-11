/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerAux.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaja <yaja@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 14:36:25 by yanaranj          #+#    #+#             */
/*   Updated: 2026/01/11 10:10:40 by yaja             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Channel *Server::moveCreateChannel(const std::string &channelName){
	for(size_t i = 0; i < _channels.size(); i++){
		if (_channels[i].getName() == channelName)
			return &_channels[i];
	}
	_channels.push_back(Channel(channelName));
	return &_channels.back();
}

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

void Server::close_fds(std::vector<pollfd> &pollFds){
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
	std::string message = cli->createMessage();
	broadcastQuit(cli, message, "Leaving");
	shutdown(fd, SHUT_RDWR);
	close(fd);
	std::cout << PURPLE << "<" << fd << "> Disconnected!" << NC << std::endl;
}
