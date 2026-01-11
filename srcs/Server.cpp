/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yanaranj <yanaranj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 16:35:09 by yanaranj          #+#    #+#             */
/*   Updated: 2026/01/11 18:10:20 by yanaranj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server() : _port(0), _servFd(-1), _serverName("ircserv") {}
bool Server::_sigFlag = false;

void Server::sigHandler(int signum)
{
	(void)signum;
	_sigFlag = true;
}
void Server::checkNewClient(){
	sockaddr_in clientAddr;
	socklen_t	clientLen = sizeof(clientAddr);
	int clientFd = accept(_servFd, (sockaddr*)&clientAddr, &clientLen);
	if (clientFd == -1){
		std::cout << RED;
		std::perror("!Error: accept()");
		std::cout << NC;
		return ;
	}
	if (fcntl(clientFd, F_SETFL, O_NONBLOCK) == -1){
		::close(_servFd);
		throw std::runtime_error("!Error: fcntl(O_NONBLOCK)");
	}
	pollfd clientPollfd = {clientFd, POLLIN, 0};
	Client newclient;
	newclient.setClientFd(clientFd);
	newclient.setClientIP(inet_ntoa(clientAddr.sin_addr));
	
	_pollFds.push_back(clientPollfd);
	_clients.push_back(newclient);
	
	std::cout << TURQUOISE << "<" << clientFd << "> Connected!" << NC << std::endl;//del?
}

void Server::checkNewData(int fd){
	char buffer[1024];
	std::memset(buffer, 0, sizeof(buffer));
	
	ssize_t bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);
	Client *cli = getClient(fd);
	std::vector<std::string> vec;
	if (bytes == 0){
		handleQuit(cli, vec);
		return ;
	}
	if (bytes < 0){
		clearClient(fd);
		return ;
	}
	if (!cli)
		return ;
	cli->addBuffer(std::string(buffer, bytes));
	std::string &buff = cli->getBuff();
	size_t pos;
	
	while((pos = buff.find("\r\n")) != std::string::npos){
		std::string cmd = buff.substr(0, pos);
		buff.erase(0, pos + 2);
		if (!cmd.empty()){
			parseCommand(cli, cmd);
		}
	}
}

void Server::createSocket()
{
	int opt = 1;
	struct sockaddr_in add;
	struct pollfd serverPollFd;

	add.sin_family = AF_INET;
	add.sin_addr.s_addr = INADDR_ANY;
	add.sin_port = htons(this->_port);

	this->_servFd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_servFd < 0){
		throw(std::runtime_error("Error creating socket"));
		close(_servFd);
	}
	if (setsockopt(_servFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1){
		throw(std::runtime_error("Error setting reuse to socket"));
		close(_servFd);
	}
	if (fcntl(_servFd, F_SETFL, O_NONBLOCK) == -1){
		throw(std::runtime_error("Error setting non-block to socket"));
		close(_servFd);
	}
	if (bind(_servFd, (struct sockaddr *)&add, sizeof(add)) < 0){
		throw(std::runtime_error("Error binding socket"));
		close(_servFd);
	}
	if (listen(_servFd, 10) == -1){
		throw(std::runtime_error("listen() crashed! :("));
		close(_servFd);
	}
	serverPollFd.fd = _servFd;
	serverPollFd.events = POLLIN;
	serverPollFd.revents = 0;
	_pollFds.push_back(serverPollFd);
}

void Server::initServer(int port, std::string pwd){
	this->_port = port;
	this->_pwd = pwd;

	createSocket();
	std::cout << GREEN << "** IRC Server Created! **\n";
	std::cout << GREEN << "\tlistering on port: " << _port << NC << std::endl;

	while (Server::_sigFlag == false){
		int activity = poll(_pollFds.data(), _pollFds.size(), -1);
		if (activity == -1 && Server::_sigFlag == false)
			throw std::runtime_error("poll() crashed! :(");
		for (int i = _pollFds.size() - 1; i >= 0; --i){
			if (!(_pollFds[i].revents & POLLIN))
				continue;
			if (_pollFds[i].fd == _servFd)
				checkNewClient();
			else
				checkNewData(_pollFds[i].fd);
		}
		for (int i = static_cast<int>(_clients.size()) - 1; i >= 0; --i){
			if (_clients[i].isToRemove()){
				int fd = _clients[i].getClientFd();
				for (size_t j = 0; j < _pollFds.size(); ++j){
					if (_pollFds[j].fd == fd){
						_pollFds.erase(_pollFds.begin() + j);
						break;
					}
				}
				for (size_t j = 0; j < _channels.size(); ){
					_channels[j].removeClient(fd);
					if (_channels[j].getClients().empty()){
						_channels.erase(_channels.begin() + j);
						continue;
					}
					++j;
				}
				_clients.erase(_clients.begin() + i);
			}
		}
	}
	close_fds(_pollFds);
}

void Server::parseCommand(Client *cli, const std::string &command){
	if (command.empty())
		return;
	if (cli->getStatus() != AUTHENTICATED){
		handShake(cli, command);
		return;
	}
	std::vector<std::string> tokens = Utils::split(command, ' ');
	if (tokens.empty())
		return;
	std::string cmd = tokens[0];
	for (size_t i = 0; i < cmd.size(); i++)
		cmd[i] = std::toupper(cmd[i]);

	switch (isCommand(cmd)){
		case PASS: passAuth(cli, tokens); break;
		case NICK: nickAuth(cli, tokens); break;
		case USER: break;
		case JOIN: handleJoin(cli, tokens); break;
		case PRIVMSG: handlePrivmsg(cli, tokens); break;
		case PART: handlePart(cli, tokens); break;
		case KICK: handleKick(cli, tokens); break;
		case MODE: handleMode(cli, tokens); break;
		case INVITE: handleInvite(cli, tokens); break;
		case QUIT: handleQuit(cli, tokens); break;
		case TOPIC: handleTopic(cli, tokens); break;
		case WHO: handleWho(cli, tokens); break;
		case PING: handlePing(cli, tokens); break;
		case NOTICE: handleNotice(cli, tokens); break;
		case UKNW: sendResponse(cli->getClientFd(), ERR_UNKNOWNCOMMAND(cmd)); break;
		default:
			break;
	}
}

CommandType Server::isCommand(const std::string &cmd){
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
	else if (cmd == "QUIT")
		return (QUIT);
	else if (cmd == "PING")
		return (PING);
	else if (cmd == "CAP")
		return (CAP);
	else if (cmd == "NOTICE")
		return (NOTICE);
	else
		return UKNW;
}
