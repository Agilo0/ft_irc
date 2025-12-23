/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alounici <alounici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 14:36:25 by yanaranj          #+#    #+#             */
/*   Updated: 2025/12/23 19:06:59 by alounici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"
#include "Utils.hpp"

Server::Server() : _port(0), _servFd(-1) , _serverName("IRC Server"){}

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
void Server::checkNewClient(){//we have to add this client to the list
	sockaddr_in clientAddr;
	socklen_t	clientLen = sizeof(clientAddr);
	int clientFd = accept(_servFd, (sockaddr*)&clientAddr, &clientLen);
	if (clientFd == -1){
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return  ;
		std::cout << RED;
		std::perror("!Error: accept.");
		std::cout << NC;
	}
	if (fcntl(_servFd, F_SETFL, O_NONBLOCK) == -1){
		::close(_servFd);
		throw std::runtime_error("!Error: NONBLOCK");
	}
	std::cout << "New client accepted!" << std::endl;
	
	pollfd clientPollfd = {clientFd, POLLIN, 0};//new poll and its struct
	
	Client newclient;
	newclient.setClientFd(clientFd);
	//newclient.setClientInfo("Uname", "Nname");
	newclient.setClientIP(inet_ntoa(clientAddr.sin_addr));//this how we set the IP
	
	_pollFds.push_back(clientPollfd);
	_clients.push_back(newclient);
	
	std::cout << PURPLE << "<" << clientFd << "> Connected!" << NC << std::endl;
	std::cout << "IP: " << newclient.getClientIP() << std::endl;
}

void Server::checkNewData(int fd){//this will give us the commands that are sending the clients
	std::cout << YELLOW << "NEW DATA FUNCTION:" << std::endl;
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));//clears the buffer
	
	int bytes = recv(fd, buffer, sizeof(buffer), 0);
	if (bytes <= 0) //is the client disconnected?
	{
		std::cout << "flag_3\n";
		close(fd);//should be marked TO REMOVE LATER
		//pollFds.erase(pollFds.begin());
		//_clients.erase(_clients.begin() + (i - 1));
		return ;
	}
	std::cout << "flag_4\n";
	Client *cli = getClient(fd);
	if (!cli)
		return ;
	cli->addBuffer(std::string(buffer, bytes));
	std::string &buff = cli->getBuff();
	
	size_t pos;

	//vvvvv we will process only a complete line
	//from here we'll work the commands
	while((pos = buff.find("\r\n")) != std::string::npos){
		std::string cmd = buff.substr(0, pos);
		buff.erase(0, pos + 2);
		if (!cmd.empty()){
			std::cout << YELLOW << "<" << fd << ">: command " << cmd << NC << std::endl;
			parseCommand(cli, cmd);
		}
	}
}

void Server::initServer(int port, std::string pwd){

	this->_port = port;
	this->_pwd = pwd;
	//std::vector<pollfd> pollFds;

	createSocket();
	
	std::cout << GREEN << "IRC Server Created!!" << std::endl;
	std::cout << "Listening on port: " << this->_port << NC << std::endl;
	
	//clientQueue();
	std::cout << "Waiting for client..." << std::endl;
	while (Server::_sigFlag == false)
	{
		int activity = poll(_pollFds.data(), _pollFds.size(), -1);
		if (activity == -1)
			throw std::runtime_error("poll() crashed! :(");//with throw theres no need to break the loop
		for (int i = _pollFds.size()-1; i >= 0; --i){
			if (!(_pollFds[i].revents & POLLIN))
				continue;
			if (_pollFds[i].fd == _servFd)//new client
				checkNewClient();
			else
				checkNewData(_pollFds[i].fd);
		}
		//std::cout << PURPLE << "Poll array ends\n";
	}
	//W.I.P
	close_fds(_pollFds);
	
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
/* 	if (cli->getStatus() != AUTHENTICATED){
		//handle authentication client commands
		return ;
	} */
	//handle other commands
	std::vector <std::string> tokens = Utils::split(command, ' ');
	if (tokens.empty())
		return ;
	std::string cmd = tokens[0];
	for(size_t i = 0; i < cmd.size(); i++){//++i?
		cmd[i] = std::toupper(cmd[i]);
	}
	
	//client commands (Agi's part)
	
	switch (isCommand(cmd))
	{
		std::cout << ORANGE << "switch statement commands" << std::endl;
		case JOIN: handleJoin(cli, tokens); break;//do we need a code for error handle???
		case PRIVMSG: handlePrivmsg(cli, tokens); break;
		//case WHO: handleWho(cli, tokens); break;	//what exactly who do?
	
		case PASS: passAuth(cli, tokens, _serverName); break;
		case NICK: nickAuth(cli, tokens, _serverName); break;
		case USER: userAuth(cli, tokens, _serverName); break;
		case UKNW: std::cerr << RED << "Unknown command for IRC \r\n" << NC << std::endl;
	default:
		break;
	}
}
CommandType Server::isCommand(const std::string &cmd){
	
	//Add here what's left
	std::cout << ORANGE << cmd << std::endl;
	if (cmd == "JOIN") return (JOIN);
	else if (cmd == "WHO") return (WHO);
	else if (cmd == "PRIVMSG") return (PRIVMSG);
	else if (cmd == "KICK") return (KICK);
	else if (cmd == "INVITE") return (INVITE);
	else if (cmd == "TOPIC") return (TOPIC);
	else if (cmd == "MODE") return (MODE);
	else if (cmd == "PASS") return (PASS);
	else if (cmd == "NICK") return (NICK);
	else if (cmd == "USER") return (USER);
	else if (cmd == "CAP") return (CAP);
	else
		return UKNW;
}

