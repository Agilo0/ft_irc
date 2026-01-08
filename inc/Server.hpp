/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alounici <alounici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 16:35:09 by yanaranj          #+#    #+#             */
/*   Updated: 2026/01/08 19:42:04 by alounici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include "Client.hpp"
#include "Channel.hpp"
#include "Utils.hpp"
#include "Responses.hpp"

class Server
{
    private:
        int _port;
		int _servFd;
		static bool _sigFlag;//This flag will be set to true when a SIGINT is received
        std::string _pwd;//server password
        std::vector<Client> _clients;
		std::vector<struct pollfd> _pollFds;
		std::vector<Channel> _channels;
		std::string _serverName;
		

		//yaja
		/*DEL LATER*/
		void printChannels(const std::vector<Channel>& _channels);
		void printClients(const std::vector<Client>& _clients);
		
		
		/*others*/
		Client *getClient(int fd);
		Client *getClientByNick(const std::string &dest);
		
		void parseCommand(Client *cli, const std::string &commad);//this handle the commands
		CommandType isCommand(const std::string &cmd);//it will give us the code, so we can work with switch statement
		void sendResponse(int client_fd, const std::string &response);


		//channels
		Channel* moveCreateChannel(const std::string &channelName);
		
		/*		--	COMMANDS	--		*/
		void handleJoin(Client *cli, const std::vector<std::string> &tokens);
		void handlePrivmsg(Client *cli, const std::vector<std::string> &tokens);
		//void handleWho(Client *cli, const std::vector<std::string> &tokens);
		void handShake(Client *cli, const std::string &command);
		void handlePart(Client *cli, const std::vector<std::string> &tokens);
		void handleKick(Client *cli, std::vector<std::string> &tokens);
		void handleMode(Client *cli, std::vector<std::string> &tokens);
		void handleInvite(Client *cli, std::vector<std::string> &tokens);
		void handleQuit(Client *cli, std::vector<std::string> &tokens);
		void handleTopic(Client *cli, std::vector<std::string> &tokens);
		void handleWho(Client *cli, std::vector<std::string> &tokens);
		void handlePing(Client *cli, std::vector<std::string> &tokens);


    public:
		//yaja
        Server();

        void initServer(int port, std::string pwd);
		void createSocket();
		static void sigHandler(int signum);
		
		/*Client*/
		void checkNewClient();
		void checkNewData(int fd);
        
		void close_fds(std::vector<pollfd> &pollFds);



		//authentification
		void passAuth(Client *cli, const std::vector<std::string> &tokens);
		void nickAuth(Client *cli, const std::vector<std::string> &tokens);
		void userAuth(Client *cli, const std::vector<std::string> &tokens);
		

		//utils
		bool checkKick(std::vector<std::string> &tokens);
		bool checkSyn(std::string channel);
		bool checkNick(std::string nick);
		bool nickTaken(std::string nick) const;
		bool channelExist(std::string channel);
		bool emitorInChannel(int emitFd, std::string channel);
		bool emitorOperator(int fd, std::string channel);
		bool isChangeMode(std::string mode);
		bool validMode(std::string mode);
		bool targetInChannel(std::string channel, int targetFd);
		int targetExist(std::string nick);
		void removeChannel(std::string chan);
		void removeTarget(std::string channel, int targetFd);
		Channel *findChannel(std::string channel);
		int findTarget(std::string nick);
		// Client *getClient(int fd);
		std::string appendToks( const std::vector<std::string> &tokens, int index);
		std::string getServerName();
		void deleteClient(Client* cli);
		int whoType(std::string cmd);
		std::string buildWhoMessage(int fd, bool op);



		//notifications
		std::vector<int> notifChannelQuit(Channel *chan, std::string message, std::string reason, std::vector<int> ok);
		std::vector<int> notifChannel(Channel *chan, std::string old, std::string nick, std::vector<int> ok);
		void broadcastTopic(std::string message, Channel *channel, std::string topic);
		void broadcastMode(Channel *chann, std::string message);
		void broadcastQuit(Client *cli, std::string message, std::string reason);
		void broadcastKick(std::string chann, std::string message, std::string nick, std::string reason);
		void broadcastPart(std::string chann, std::string message, std::string reason);
		void broadcastNewNick(Client *cli);


};

#endif