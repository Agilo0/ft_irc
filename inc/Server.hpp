/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yanaranj <yanaranj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 16:35:09 by yanaranj          #+#    #+#             */
/*   Updated: 2025/11/26 16:58:03 by yanaranj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include "Utils.hpp"

class Server
{
    private:
        int _port;
        std::string _pwd;
//        int _fd;
		int _servFd;

		//yaja
		static bool _sigFlag;//This flag will be set to true when a SIGINT is received

    public:
		//yaja
        Server();
		static void sigHandler(int signum);
        void initServer(int port, std::string pwd);
		void createSocket();

};

#endif