/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alounici <alounici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 14:36:25 by yanaranj          #+#    #+#             */
/*   Updated: 2025/11/28 21:24:13 by alounici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"


Server::Server() : _port(0), _servFd(-1) {}

bool Server::_sigFlag = false;//no signal received yet

void Server::sigHandler(int signum)
{
	(void)signum;
	_sigFlag = true;
}

void Server::createSocket()
{
	struct sockaddr_in add;//used to store socket addresses for the Internet domain
	//struct socket
	add.sin_family = AF_INET; //specify the IPv4 address protocol
	add.sin_addr.s_addr = INADDR_ANY; //accept connections from any IP address
	add.sin_port = htons(this->_port); //convert port number to network byte order
	
	//create socket
	this->_servFd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_servFd < 0)
	{
		std::cerr << RED << "Error creating socket" << std::endl;
		std::cerr << "Error:" << strerror(errno) << NC << std::endl;
		exit(EXIT_FAILURE);
	}
}


void Server::initServer(int port, std::string pwd){
	this->_port = port;
	this->_pwd = pwd;

	createSocket();


	clientQueue();
}

void Server::clientQueue()
{

	std::vector<pollfd> pollFds;

	pollfd serverPollFd;
	serverPollFd.fd = _servFd;
	serverPollFd.events = POLLIN;
	pollFds.push_back(serverPollFd);

	std::cout << "waiting for client..." << std::endl;
	while (1)
	{
		int activity = poll(pollFds.data(), pollFds.size(), -1);
		if (activity == -1)
		{
			std::cerr << "poll() crashed! :(" << std::endl;
			break;
		}
		unsigned long i = 0;
		while (i < pollFds.size())
		{
			if (_sigFlag == true)
			{
				std::cerr << "Exiting..." << std::endl;
				return ;
			}
			i = client_event(pollFds, i);
			i++;
		}
	}
}

unsigned long Server::client_event(std::vector<pollfd> &pollFds, unsigned long i)
{
	if (pollFds[i].revents & POLLIN)
	{
		if (pollFds[i].fd == _servFd)
		{
			sockaddr_in clientAddr;
			socklen_t	clientLen = sizeof(clientAddr);
			int clientFd = accept(_servFd, (sockaddr*)&clientAddr, &clientLen);
			if (clientFd == -1)
			{
				std::cerr << "Error accepting new client. :(" << std::endl;
				return (i); 
			}
			std::cout << "New client connected!" << std::endl;
			pollfd clientPollfd = {clientFd, POLLIN, 0};
			pollFds.push_back(clientPollfd);
			Client newclient(clientFd);
			_clients.push_back(newclient);
		}
		else
		{
			char buffer[1024];
			
			int bytes = recv(pollFds[i].fd, buffer, sizeof(buffer), 0);
			if (bytes <= 0)
			{
				close(pollFds[i].fd);
				pollFds.erase(pollFds.begin() + i);
				_clients.erase(_clients.begin() + (i - 1));
				i--;
			}
			else
			{
				std::string msg(buffer, bytes);
				manage_msg(msg, i);
			}
		}
	}
	return (i);
}




void Server::manage_msg(std::string msg, int index)
{
	(void)msg;
	(void)index;
}
