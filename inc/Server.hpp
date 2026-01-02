/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yanaranj <yanaranj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 16:35:09 by yanaranj          #+#    #+#             */
/*   Updated: 2026/01/02 12:46:05 by yanaranj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include "Utils.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Responses.hpp"

class Server{
    private:
		int _port;
		int _servFd;
		static bool _sigFlag;
		std::string _pwd;
		std::string _serverName;
		std::vector<struct pollfd> _pollFds;
		std::vector<Client> _clients;
		//std::vector<Channel> _channels:

	//utils.cpp
		bool checkNick(std::string nick);
		bool checkUser(std::string user) const;
		bool nickTaken(std::string nick) const;
		//void broadcastNewNick(Client *cli);//works with channels
		std::string appendToks( const std::vector<std::string> &tokens, int start);

	//Authentication
		void handShake(Client *cli, const std::string &command);
		void passAuth(Client *cli, const std::vector<std::string> &tokens);
		void nickAuth(Client *cli, const std::vector<std::string> &tokens);
		void userAuth(Client *cli, const std::vector<std::string> &tokens);
	
	//commands
		Client *getClient(int fd);
		void parseCommand(Client *cli, const std::string &command);
		CommandType isCommand(const std::string &cmd);
		void sendResponse(int clientFd, const std::string &response);
		
    public:
		Server();
	//server.cpp
		void initServer(int port, std::string pwd);
		void createSocket();
		
		static void sigHandler(int signum);
		void checkNewClient();
		void checkNewData(int fd);
		void clearClient(int fd);
		void close_fds(std::vector<pollfd> &pollFds);
;

	
};

#endif
