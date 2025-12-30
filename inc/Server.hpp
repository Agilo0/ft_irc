/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alounici <alounici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 16:35:09 by yanaranj          #+#    #+#             */
/*   Updated: 2025/12/30 20:18:09 by alounici         ###   ########.fr       */
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
		void handleKick(Client *cli, std::vector<std::string> &tokens);
		void handleMode(Client *cli, std::vector<std::string> &tokens);


    public:
		//yaja
        Server();

        void initServer(int port, std::string pwd);
		void createSocket();
		static void sigHandler(int signum);
		
		/*Client*/
		void checkNewClient();
		void checkNewData(int fd);
        
        //agi
        //void clientQueue();
        unsigned long client_event(std::vector<pollfd> &pollfds, unsigned long i);
		void close_fds(std::vector<pollfd> &pollFds);



		//authentification
		void passAuth(Client *cli, const std::vector<std::string> &tokens);
		void nickAuth(Client *cli, const std::vector<std::string> &tokens);
		void userAuth(Client *cli, const std::vector<std::string> &tokens);
		void broadcastNewNick(Client *cli);
		std::vector<int> notifChannel(Channel *chan, std::string old, std::string nick, std::vector<int> ok);
		
		
		//part
		void handlePart(Client *cli, const std::vector<std::string> &tokens);
		void broadcastPart(std::string chann, std::string message, std::string reason);

		//kick
		bool checkKick(std::vector<std::string> &tokens);
		void removeTarget(std::string channel, int targetFd);
		void broadcastKick(std::string chann, std::string message, std::string nick, std::string reason);


		//utils
		std::string appendToks( const std::vector<std::string> &tokens, int index);
		bool checkSyn(std::string channel);
		std::string getServerName();
		bool checkNick(std::string nick);
		bool checkUser(std::string user) const;
		bool nickTaken(std::string nick) const;
		bool channelExist(std::string channel);
		void removeChannel(std::string chan);
		bool emitorInChannel(int emitFd, std::string channel);
		bool emitorOperator(int fd, std::string channel);
		int targetExist(std::string nick);
		bool targetInChannel(std::string channel, int targetFd);
		Channel *findChannel(std::string channel);
		bool isChangeMode(std::string mode);
		bool validMode(std::string mode);
		int findTarget(std::string nick);
		void broadcastMode(Channel *chann, std::string message);






};

#endif