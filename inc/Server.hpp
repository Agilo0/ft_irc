/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaja <yaja@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 16:35:09 by yanaranj          #+#    #+#             */
/*   Updated: 2026/01/01 20:12:18 by yaja             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include "Utils.hpp"
#include "Client.hpp"
//#include "Channel.hpp"
//#include "Responses.hpp"

class Server{
    private:
		int _port;
		int _servFd;
		static bool _sigFlag;
		std::string _pwd;
		std::string _serverName;
		std::vector<struct pollfd> _pollFds;
		std::vector<Client> _clients;
		//std::vector<Channel> _channels;

    public:
	//serverAux.cpp
		static void sigHandler(int signum);
		void checkNewClient();
		void checkNewData(int fd);
		//void close_fds();//para el final!!
	//server.cpp
		void initServer(int port, std::string pwd);
		void createSocket();
	
	//server close
		void clearClient(int fd);
		void close_fds(std::vector<pollfd> &pollFds);

	//NEXT TO DOO!!!
		void parseCommand(Client *cli, const std::string &command);
		Client *getClient(int fd);
	
};

#endif
