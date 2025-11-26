/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yanaranj <yanaranj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 14:36:25 by yanaranj          #+#    #+#             */
/*   Updated: 2025/11/26 16:59:58 by yanaranj         ###   ########.fr       */
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
}