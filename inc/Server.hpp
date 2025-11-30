/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaja <yaja@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 16:35:09 by yanaranj          #+#    #+#             */
/*   Updated: 2025/11/30 13:31:16 by yaja             ###   ########.fr       */
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
		void close_fds(std::vector<pollfd> &pollFds);


};

#endif