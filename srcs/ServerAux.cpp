/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerAux.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaja <yaja@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 14:36:25 by yanaranj          #+#    #+#             */
/*   Updated: 2026/01/01 20:14:22 by yaja             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"
#include "Utils.hpp"

Server::Server() : _port(0), _servFd(-1) , _serverName("ircserv"){}

bool Server::_sigFlag = false;//no signal received yet

void Server::sigHandler(int signum)
{
	(void)signum;
	std::cout << PURPLE << "Flag is true! " << signum << NC << std::endl;
	_sigFlag = true;
}

void Server::checkNewClient(){//we have to add this client to the list
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
	if (bytes == 0) //is the client disconnected?
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
	
	//vvvvv we will process only a complete line
	//while((pos = buff.find("\r\n")) != std::string::npos){ //the real loop should be like this. But netcat can work different in other OS
	while((pos = buff.find("\n")) != std::string::npos){
		std::cout << BLUE << "CheckNewData loop\n" << NC;
		
		std::string cmd = buff.substr(0, pos);
		buff.erase(0, pos + 2);
		if (!cmd.empty()){
			std::cout << YELLOW << "<" << fd << "> << " << cmd << NC << std::endl;
			parseCommand(cli, cmd);
		}
	}
}