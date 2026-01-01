/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaja <yaja@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 16:35:09 by yanaranj          #+#    #+#             */
/*   Updated: 2026/01/01 20:12:40 by yaja             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
//NEXT STEP:
/*
	I need to redo again the client part. So let´s build it´s informatiobn again!!!
	So lets start with getClient() and the we go to commands!!!
*/

Server::Server() : _port(0), _servFd(-1) , _serverName("ircserv"){}

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

//parsing commands