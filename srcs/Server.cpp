/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaja <yaja@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 16:35:09 by yanaranj          #+#    #+#             */
/*   Updated: 2026/01/08 13:12:19 by yaja             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/*CRASH WHEN WE CLOSE HEXCHAT. STILL WAITING FOR JOIN AND MORE COMMANDS*/

Server::Server() : _port(0), _servFd(-1), _serverName("ircserv") {}
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

void Server::createSocket()
{
	int opt = 1;
	struct sockaddr_in add; // used to store socket addresses for the Internet domain
	struct pollfd serverPollFd;

	// socket struct
	add.sin_family = AF_INET;		   // specify the IPv4 address protocol
	add.sin_addr.s_addr = INADDR_ANY;  // accept connections from any IP address
	add.sin_port = htons(this->_port); // convert port number to network byte order

	// create socket
	this->_servFd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_servFd < 0)
	{
		throw(std::runtime_error("Error creating socket"));
		close(_servFd);
	}
	// addresses reuse (to restart the server quickly)
	if (setsockopt(_servFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		throw(std::runtime_error("Error setting reuse to socket"));
		close(_servFd);
	}
	// prevent to block incoming connections, to avoid to freeze the program with one client
	if (fcntl(_servFd, F_SETFL, O_NONBLOCK) == -1)
	{
		throw(std::runtime_error("Error setting non-block to socket"));
		close(_servFd);
	}
	// bind socket to an IP + port
	if (bind(_servFd, (struct sockaddr *)&add, sizeof(add)) < 0)
	{
		throw(std::runtime_error("Error binding socket"));
		close(_servFd);
	}
	if (listen(_servFd, 10) == -1)
	{
		throw(std::runtime_error("listen() crashed! :("));
		close(_servFd);
	}

	// poll is the one who monitors the socket
	serverPollFd.fd = _servFd;
	serverPollFd.events = POLLIN; // requested events
	serverPollFd.revents = 0;	  // returned events
	_pollFds.push_back(serverPollFd);
}

void Server::initServer(int port, std::string pwd)
{
	this->_port = port;
	this->_pwd = pwd;

	createSocket();

	std::cout << GREEN << "** IRC Server Created! **\n";
	std::cout << GREEN << "\tlistering on port: " << _port << NC << std::endl;

	while (Server::_sigFlag == false)
	{
		int activity = poll(_pollFds.data(), _pollFds.size(), -1);
		if (activity == -1)
			throw std::runtime_error("poll() crashed! :(");
		for (int i = _pollFds.size() - 1; i >= 0; --i)
		{
			if (!(_pollFds[i].revents & POLLIN))
				continue;
			if (_pollFds[i].fd == _servFd)
				checkNewClient();
			else
				checkNewData(_pollFds[i].fd);
		}
		// in order to avoid any further issue, we'll mark the clients for remove later
		for (int i = static_cast<int>(_clients.size()) - 1; i >= 0; --i)
		{
			if (_clients[i].isToRemove())
			{
				int fd = _clients[i].getClientFd();
				// look and delete from _pollFds
				for (size_t j = 0; j < _pollFds.size(); ++j)
				{
					if (_pollFds[j].fd == fd)
					{
						_pollFds.erase(_pollFds.begin() + j);
						break;
					}
				}
				// now let´s delete from channels
				for (size_t j = 0; j < _channels.size(); ){
					_channels[j].removeClient(fd);
					if (_channels[j].getClients().empty()){ //if is the only member, delete channel too
						_channels.erase(_channels.begin() + j);
						continue; //nothing to increment cause vector has delete a space
					}
					++j;
				}
				_clients.erase(_clients.begin() + i);
			}
		}
		// print clients and channels
	}
	close_fds(_pollFds);
}

void Server::parseCommand(Client *cli, const std::string &command)
{
	if (command.empty())
		return;
	// if it´s the 1st time, we need to verify his identity
	if (cli->getStatus() != AUTHENTICATED)
	{
		handShake(cli, command);
		return;
	}
	// handle other commands
	std::vector<std::string> tokens = Utils::split(command, ' ');
	if (tokens.empty())
		return;
	std::string cmd = tokens[0];
	for (size_t i = 0; i < cmd.size(); i++)
		cmd[i] = std::toupper(cmd[i]);

	switch (isCommand(cmd)){
		std::cout << ORANGE << "switch state commands" << std::endl;
		
		case PASS: passAuth(cli, tokens); break;
		case NICK: nickAuth(cli, tokens); break;
		case USER: break;//the user is only a register thing
		case JOIN: handleJoin(cli, tokens); break;
		//WHO
		case PRIVMSG: handlePrivmsg(cli, tokens); break;
		case PART: handlePart(cli, tokens); break;
		
		case UKNW: sendResponse(cli->getClientFd(), ERR_UNKNOWNCOMMAND(_serverName, cli->getNickname(), cmd)); break;
		default:
			break;
	}
	/* switch (isCommand(cmd))
	{
		//case WHO: handleWho(cli, tokens); break;	//what exactly who do?

		case KICK: handleKick(cli, tokens); break;
		case MODE: handleMode(cli, tokens); break;
		case INVITE: handleInvite(cli, tokens); break;
	default:
		break;
	} */
}

CommandType Server::isCommand(const std::string &cmd)
{

	std::cout << ORANGE << cmd << std::endl;
	if (cmd == "JOIN")
		return (JOIN);
	else if (cmd == "WHO")
		return (WHO);
	else if (cmd == "PRIVMSG")
		return (PRIVMSG);
	else if (cmd == "KICK")
		return (KICK);
	else if (cmd == "INVITE")
		return (INVITE);
	else if (cmd == "TOPIC")
		return (TOPIC);
	else if (cmd == "MODE")
		return (MODE);
	else if (cmd == "PASS")
		return (PASS);
	else if (cmd == "NICK")
		return (NICK);
	else if (cmd == "USER")
		return (USER);
	else if (cmd == "PART")
		return (PART);
	else
		return UKNW;
}