/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaja <yaja@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 16:35:09 by yanaranj          #+#    #+#             */
/*   Updated: 2026/01/09 12:16:32 by yaja             ###   ########.fr       */
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
		std::vector<Channel> _channels;


	//DETELE THEM
	void printChannels(const std::vector<Channel>& _channels);
	void printClients(std::vector<Client>& _clients);



	//utils.cpp
		bool checkNick(std::string nick);
		bool checkUser(std::string user) const;
		bool nickTaken(std::string nick) const;
		std::string appendToks( const std::vector<std::string> &tokens, int start);
		bool checkSyn(std::string channel);
		bool channelExist(std::string channel);
		void removeChannel(std::string chan);
		bool checkKick(std::vector<std::string> &tokens);
		bool emitorInChannel(int emitFd, std::string channel);
		bool emitorOperator(int fd, std::string channel);
		bool targetInChannel(std::string channel, int targetFd);
		int targetExist(std::string nick);
		void removeTarget(std::string channel, int targetFd);
		bool isChangeMode(std::string mode);
		bool validMode(std::string mode);
		Channel *findChannel(std::string channel);
		int findTarget(std::string nick);


	//ServerAuth.cpp
		void passAuth(Client *cli, const std::vector<std::string> &tokens);
		void nickAuth(Client *cli, const std::vector<std::string> &tokens);
		void userAuth(Client *cli, const std::vector<std::string> &tokens);
		void handShake(Client *cli, const std::string &command);
	
	//Server.cpp (COMMANDS)
		void parseCommand(Client *cli, const std::string &command);
		void handleJoin(Client *cli, const std::vector<std::string> &tokens);
		void handlePrivmsg(Client *cli, const std::vector<std::string> &tokens);
		void handlePart(Client *cli, const std::vector<std::string> &tokens);
		void handleKick(Client *cli, std::vector<std::string> &tokens);

		void handleMode(Client *cli, std::vector<std::string> &tokens);
		void handleInvite(Client *cli, std::vector<std::string> &tokens);
		void handleQuit(Client *cli, std::vector<std::string> &tokens);
		void handleTopic(Client *cli, std::vector<std::string> &tokens);
		void handleWho(Client *cli, std::vector<std::string> &tokens);
	
	//Client.cpp
		CommandType isCommand(const std::string &cmd);
		void sendResponse(int clientFd, const std::string &response);
	
	//ServerAux.cpp
		Channel* moveCreateChannel(const std::string &channelName);
		Client *getClient(int fd);
		Client *getClientByNick(const std::string &dest);

	//Notification.cpp
		void broadcastNewNick(Client *cli);//works with channels
		std::vector<int> notifChannel(Channel *chan, std::string old, std::string nick, std::vector<int> ok);
		void broadcastPart(std::string chann, std::string message, std::string reason);
		void broadcastKick(std::string chann, std::string message, std::string nick, std::string reason);
		void broadcastMode(Channel *chann, std::string message);
		void broadcastQuit(Client *cli, std::string message, std::string reason);
		
	
	//ServerCommands.cpp
		
    public:
		Server();
	//server.cpp
		void createSocket();
		void initServer(int port, std::string pwd);
	//ServerAux.cpp
		static void sigHandler(int signum);
		void checkNewClient();
		void checkNewData(int fd);
		void clearClient(int fd);
		void close_fds(std::vector<pollfd> &pollFds);

	
};

#endif
