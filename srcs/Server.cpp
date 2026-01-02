/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yanaranj <yanaranj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 16:35:09 by yanaranj          #+#    #+#             */
/*   Updated: 2026/01/02 12:51:09 by yanaranj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/*CRASH WHEN WE CLOSE HEXCHAT. STILL WAITING FOR JOIN AND MORE COMMANDS*/

Server::Server() : _port(0), _servFd(-1) , _serverName("ircserv"){}

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
    
    createSocket();

    std::cout << GREEN << "** IRC Server Created! **\n";
    std::cout << GREEN << "\tlistering on port: " << _port << NC << std::endl;

    while(Server::_sigFlag == false){
        int activity = poll(_pollFds.data(), _pollFds.size(), -1);
		if (activity == -1)
			throw std::runtime_error("poll() crashed! :(");
		for (int i = _pollFds.size()-1; i >= 0; --i){
			if (!(_pollFds[i].revents & POLLIN))
				continue;
			if (_pollFds[i].fd == _servFd)
				checkNewClient();
			else
				checkNewData(_pollFds[i].fd);
		}
        //in order to avoid any further issue, we'll mark the clients for remove later
        for (int i = static_cast<int>(_clients.size()) - 1; i >= 0; --i){
            if (_clients[i].isToRemove()){
                int fd = _clients[i].getClientFd();
                //look and delete from _pollFds
                for (size_t j = 0; j < _pollFds.size(); ++j){
                    if (_pollFds[j].fd == fd){
                        _pollFds.erase(_pollFds.begin() + j);
                        break;
                    }
                }
				//now let´s delete from channels
				/* for (size_t j = 0; j < _channels.size(); ){
					_channels[j].removeClient(fd);
					if (_channels[j].getClients().empty()){ //if is the only member, delete channel too
						_channels.erase(_channels.begin() + j);
						continue; //nothing to increment cause vector has delete a space
					}
					++j;
				} */
				_clients.erase(_clients.begin() + i);
            }
        }
		//print clients and channels
    }
	close_fds(_pollFds);
}

void Server::parseCommand(Client *cli, const std::string &command){
	if (command.empty())
		return ;
	//if it´s the 1st time, we need to verify his identity
	if (cli->getStatus() != AUTHENTICATED){
		std::cout << "AUTHENTICATED client\n" << NC;
		handShake(cli, command);
		return ;
	}
	//handle other commands
	std::vector <std::string> tokens = Utils::split(command, ' ');
	if (tokens.empty())
		return ;
	std::string cmd = tokens[0];
	for(size_t i = 0; i < cmd.size(); i++){
		cmd[i] = std::toupper(cmd[i]);
	}
	/* switch (isCommand(cmd))
	{
		std::cout << ORANGE << "switch statement commands" << std::endl;
		case JOIN: handleJoin(cli, tokens); break;//do we need a code for error handle???
		case PRIVMSG: handlePrivmsg(cli, tokens); break;
		//case WHO: handleWho(cli, tokens); break;	//what exactly who do?
		case PASS: passAuth(cli, tokens); break;
		case NICK: nickAuth(cli, tokens); break;
		case USER: userAuth(cli, tokens); break;
		case PART: handlePart(cli, tokens); break;
		case KICK: handleKick(cli, tokens); break;
		case MODE: handleMode(cli, tokens); break;
		case INVITE: handleInvite(cli, tokens); break;
		case UKNW: std::cerr << RED << "Unknown command for IRC \r\n" << NC << std::endl;
	default:
		break;
	} */
}


CommandType Server::isCommand(const std::string &cmd){
	
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
	else if (cmd == "PART") return (PART);
	else
		return UKNW;
}