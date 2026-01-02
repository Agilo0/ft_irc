/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerAux.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yanaranj <yanaranj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 14:36:25 by yanaranj          #+#    #+#             */
/*   Updated: 2026/01/02 12:36:53 by yanaranj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"
#include "Utils.hpp"

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
		std::cout << BLUE << "CheckNewData loop\n" << NC;
		
		std::string cmd = buff.substr(0, pos);
		buff.erase(0, pos + 2);
		if (!cmd.empty()){
			std::cout << YELLOW << "<" << fd << "> << " << cmd << NC << std::endl;
			parseCommand(cli, cmd);
		}
	}
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
/* void Server::broadcastNewNick(Client *cli)
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
} */

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