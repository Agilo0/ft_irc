/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yanaranj <yanaranj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 14:36:25 by yanaranj          #+#    #+#             */
/*   Updated: 2025/12/08 15:58:07 by yanaranj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server() : _port(0), _servFd(-1) {}

bool Server::_sigFlag = false;//no signal received yet

void Server::sigHandler(int signum)
{
	(void)signum;
	_sigFlag = true;
	std::cout << PURPLE << "Flag is true! " << signum << NC << std::endl;
}

void Server::createSocket()
{
	int opt = 1;
	struct sockaddr_in add;//used to store socket addresses for the Internet domain
	struct pollfd serverPollFd;
	
	//struct socket
	add.sin_family = AF_INET; //specify the IPv4 address protocol
	add.sin_addr.s_addr = INADDR_ANY; //accept connections from any IP address
	add.sin_port = htons(this->_port); //convert port number to network byte order
	
	//create socket
	this->_servFd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_servFd < 0){
		throw(std::runtime_error("Error creating socket"));
		close(_servFd);
	}
	//addresses reuse (to restart the server quickly)
	if (setsockopt(_servFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1){
		throw(std::runtime_error("Error setting reuse to socket"));
		close(_servFd);
	}
	//prevent to block incoming connections, to avoid to freeze the program with one client
	if (fcntl(_servFd, F_SETFL, O_NONBLOCK) == -1){
		throw(std::runtime_error("Error setting non-block to socket"));
		close(_servFd);	
	}
	//bind socket to an IP + port
	if (bind(_servFd, (struct sockaddr*)&add, sizeof(add)) < 0){
		throw (std::runtime_error("Error binding socket"));
		close(_servFd);	
	}
	if (listen(_servFd, 10) == -1){
		throw(std::runtime_error("listen() crashed! :("));
		close(_servFd);	
	}
	
	//poll is the one who monitors the socket
	serverPollFd.fd = _servFd;
	serverPollFd.events = POLLIN;
	serverPollFd.revents = 0;
	_pollFds.push_back(serverPollFd);
	
}

void Server::initServer(int port, std::string pwd){

	this->_port = port;
	this->_pwd = pwd;
	//std::vector<pollfd> pollFds;

	createSocket();
	
	std::cout << GREEN << "IRC Server Created!!" << std::endl;
	std::cout << "Listeing on port: " << this->_port << NC << std::endl;
	
	//clientQueue();
	std::cout << "Waiting for client..." << std::endl;
	while (Server::_sigFlag == false)
	{
		int activity = poll(_pollFds.data(), _pollFds.size(), -1);
		if (activity == -1)
		{
			std::cerr << "poll() crashed! :(" << std::endl;
			break;
		}
		unsigned long i = 0;
		while (i < _pollFds.size())
		{
			i = client_event(_pollFds, i);
			i++;
		}
	}
	close_fds(_pollFds);
	
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
			//NOW OUR CONNECTIONS ARRIVES HERE
			std::cout << "New client accepted!" << std::endl;
			pollfd clientPollfd = {clientFd, POLLIN, 0};
			pollFds.push_back(clientPollfd);
			Client newclient(clientFd);
			_clients.push_back(newclient);
			std::cout << PURPLE << "New client added!" << NC << std::endl;
			//we have add a new client
		}
		else
		{
			char buffer[1024];
			
			int bytes = recv(pollFds[i].fd, buffer, sizeof(buffer), 0);
			if (bytes <= 0) //this should handle ^D?
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
			std::cout << BLUE << "Cannot add a new Client. :(" << NC << std::endl;
		}
	}
	return (i);
}



//esto para??
void Server::manage_msg(std::string msg, int index)
{
	(void)msg;
	(void)index;
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