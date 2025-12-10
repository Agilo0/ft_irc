/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yanaranj <yanaranj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 14:36:25 by yanaranj          #+#    #+#             */
/*   Updated: 2025/12/10 10:17:35 by yanaranj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Utils.hpp"

Server::Server() : _port(0), _servFd(-1) {}

bool Server::_sigFlag = false;//no signal received yet

void Server::sigHandler(int signum)
{
	(void)signum;
	std::cout << PURPLE << "Flag is true! " << signum << NC << std::endl;
	_sigFlag = true;
}

void Server::createSocket()
{
	int opt = 1;
	struct sockaddr_in add;//used to store socket addresses for the Internet domain
	struct pollfd serverPollFd;
	
	//socket struct
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
	serverPollFd.events = POLLIN;//requested events
	serverPollFd.revents = 0;//returned events
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
		if (pollFds[i].fd == _servFd)//new client
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
			std::cout << GREEN << "Client <" << clientFd << "> Connected" << NC << std::endl;
		}

			//we have add a new client
	}
		else//existing client
		{
			char buffer[1024];
			memset(buffer, 0, sizeof(buffer));//clears the buffer
			
			int bytes = recv(pollFds[i].fd, buffer, sizeof(buffer), 0);
			if (bytes <= 0) //is the client disconnected?
			{
				close(pollFds[i].fd);
				pollFds.erase(pollFds.begin() + i);
				_clients.erase(_clients.begin() + (i - 1));
				i--;
			}
			else
			{
				std::string msg(buffer, bytes);
				std::cout << TURQUOISE << "Message received from client <" << pollFds[i].fd << ">: " << msg << NC << std::endl;
				manage_msg(msg, i);
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


//We have to complete with Agi's commands too
void Server::parseCommand(Client *cli, const std::string &command)
{
	if (command.empty())
		return ;
	if (cli->getStatus() != AUTHENTICATED){
		//handle authentication client commands
		return ;
	}
	//handle other commands
	std::vector <std::string> tokens = split(command, ' ');
	if (tokens.empty())
		return ;
	std::string cmd = tokens[0];
	for(size_t i = 0; i < cmd.size(); i++){//++i?
		std::cout << TURQUOISE << "Converting: " << cmd[i] << NC << std::endl;
		cmd[i] = std::toupper(cmd[i]);
	}
	
	//client commands (Agi's part)
	
	switch (isCommand(cmd))
	{
		case JOIN: handleJoin(cli, tokens); break;//do we need a code for error handle???
		//case WHO: handleWho(cli, tokens); break;	//what exactly who do?
	
		case UKNW: std::cerr << RED << "Unknown command for IRC \r\n" << NC << std::endl;
	default:
		break;
	}
}
CommandType Server::isCommand(const std::string &cmd){
	
	//Add here what's left
	
	if (cmd == "JOIN") return (JOIN);
	else if (cmd == "WHO") return (WHO);
	else if (cmd == "PRIVMSG") return (PRIVMSG);
	else if (cmd == "KICK") return (KICK);
	else if (cmd == "INVITE") return (INVITE);
	else if (cmd == "TOPIC") return (TOPIC);
	else if (cmd == "MODE") return (MODE);
	else
		return UKNW;
}