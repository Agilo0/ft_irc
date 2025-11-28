/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alounici <alounici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 16:35:09 by yanaranj          #+#    #+#             */
/*   Updated: 2025/11/28 21:24:51 by alounici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP


#include "Utils.hpp"
#include "Client.hpp"
#include <poll.h>
#include <vector>
#include <iostream>

class Server
{
    private:
        int _port;
        std::string _pwd;
//        int _fd;
		int _servFd;
        std::vector<Client> _clients;

		//yaja
		static bool _sigFlag;//This flag will be set to true when a SIGINT is received

    public:
		//yaja
        Server();
		static void sigHandler(int signum);
        void initServer(int port, std::string pwd);
		void createSocket();
        
        
        //agi
        void clientQueue();
        unsigned long client_event(std::vector<pollfd> &pollfds, unsigned long i);
        void manage_msg(std::string msg, int index);


};

#endif