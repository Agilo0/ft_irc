/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerAux.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yanaranj <yanaranj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 14:36:25 by yanaranj          #+#    #+#             */
/*   Updated: 2026/01/07 19:32:27 by yanaranj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool Server::_sigFlag = false;//no signal received yet

void Server::sigHandler(int signum)
{
	(void)signum;
	std::cout << PURPLE << "Flag is true! " << signum << NC << std::endl;
	_sigFlag = true;
}

//we have to add this client to the list
void Server::checkNewClient(){
	sockaddr_in clientAddr;
	socklen_t	clientLen = sizeof(clientAddr);
	int clientFd = accept(_servFd, (sockaddr*)&clientAddr, &clientLen);
	if (clientFd == -1){
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return  ;
		std::cout << RED;
		std::perror("!Error: accept()");
		std::cout << NC;
		return ;
	}
	if (fcntl(_servFd, F_SETFL, O_NONBLOCK) == -1){
		::close(_servFd);
		throw std::runtime_error("!Error: fcntl(O_NONBLOCK)");
	}
	pollfd clientPollfd = {clientFd, POLLIN, 0};//new poll and its struct	
	Client newclient;
	newclient.setClientFd(clientFd);
	newclient.setClientIP(inet_ntoa(clientAddr.sin_addr));//this how we set the IP
	
	_pollFds.push_back(clientPollfd);
	_clients.push_back(newclient);
	
	std::cout << TURQUOISE << "<" << clientFd << "> Connected!" << NC << std::endl;
}

//this will give us the commands that are sending the clients
void Server::checkNewData(int fd){
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));//clears the buffer
	
	ssize_t bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);
	if (bytes == 0)
	{
		std::cout << "flag_3\n";
		clearClient(fd);
		return ;
	}
	else if (bytes < 0) {
    	if (errno == EAGAIN || errno == EWOULDBLOCK)
			return;
	}
	Client *cli = getClient(fd);
	if (!cli)
		return ;
	cli->addBuffer(std::string(buffer, bytes));
	std::string &buff = cli->getBuff();
	size_t pos;
	
	//while((pos = buff.find("\n")) != std::string::npos){//just for MacOS test (yaja)
	while((pos = buff.find("\r\n")) != std::string::npos){
		std::string cmd = buff.substr(0, pos);
		buff.erase(0, pos + 2);
		if (!cmd.empty()){
			std::cout << YELLOW << "<" << fd << "> << " << NC << cmd << std::endl;
			parseCommand(cli, cmd);
		}
	}
}

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
std::vector<int> Server::notifChannel(Channel *chan, std::string old, std::string nick, std::vector<int> ok){
	std::set<int> clients = chan->getClients();
	std::set<int>::iterator it = clients.begin();
	
	while (it != clients.end())
	{
		int fd = *it;
		if (std::find(ok.begin(), ok.end(), fd) == ok.end())
		{
			sendResponse(fd, RPL_NICK_UPDATE(old, nick));
			ok.push_back(fd);
		}
		it++;

	}
	return (ok);
}
void Server::close_fds(std::vector<pollfd> &pollFds)
{
	int i = pollFds.size() - 1;
	
	while (i >= 0)
	{
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