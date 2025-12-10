/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yanaranj <yanaranj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 16:35:09 by yanaranj          #+#    #+#             */
/*   Updated: 2025/12/10 11:42:58 by yanaranj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include "Utils.hpp"
#include "Client.hpp"
#include "Responses.hpp"
#include "Channel.hpp"

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
		

		//yaja
		void parseCommand(Client *cli, const std::string &commad);//this handle the commands
		CommandType isCommand(const std::string &cmd);//it will give us the code, so we can work with switch statement
		void sendResponse(int client_fd, const std::string &response);

		//channels
		Channel* moveCreateChannel(const std::string &channelName);
		
		/*		--	COMMANDS	--		*/
		void handleJoin(Client *cli, const std::vector<std::string> &tokens);

    public:
		//yaja
        Server();

        void initServer(int port, std::string pwd);
		void createSocket();
		static void sigHandler(int signum);
		
        
        //agi
        //void clientQueue();
        unsigned long client_event(std::vector<pollfd> &pollfds, unsigned long i);
        void manage_msg(std::string msg, int index);
		void close_fds(std::vector<pollfd> &pollFds);


};

#endif