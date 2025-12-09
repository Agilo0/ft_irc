/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yanaranj <yanaranj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 16:35:09 by yanaranj          #+#    #+#             */
/*   Updated: 2025/12/09 12:09:58 by yanaranj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP


#include "Utils.hpp"
#include "Client.hpp"

class Server
{
    private:
        int _port;
		int _servFd;
		static bool _sigFlag;//This flag will be set to true when a SIGINT is received
        std::string _pwd;//server password
        std::vector<Client> _clients;
		std::vector<struct pollfd> _pollFds;

		//yaja
		void parseCommand(Client *cli, const std::string &cmd);//this handle the commands
		void sendResponse(int client_fd, const std::string &response);	
		//COMMANDS
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