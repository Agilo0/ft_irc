/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaja <yaja@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 19:58:49 by yanaranj          #+#    #+#             */
/*   Updated: 2026/01/01 17:53:32 by yaja             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include "Exception.hpp"
#include "Server.hpp"
#include "Utils.hpp"
//#include "Client.hpp"


int main(int argc, char** argv)
{
    std::string line;
	signal(SIGINT, Server::sigHandler);//^C. Interrupt the process
	signal(SIGQUIT, Server::sigHandler);//^\ Quit process (9/12/25->SEGV )
	
    if (argc != 3){
        std::cerr << ORANGE << "Usage: ./ircserv <port> <password>" << std::endl;
        return (1);
    }
	int port = 0;
	std::string password = argv[2];
    try
    {
        port = Utils::check_port(argv[1]);
		if (Utils::check_password(argv[2]) == false)
			throw std::runtime_error("Error: Invalid password.");
    }
    catch(const std::exception& e)
    {
        std::cerr << RED << e.what() << '\n';
        return (1);
    }
	 
    Server Server;
    try{
	    Server.initServer(port, password);
    }
    catch (const std::exception &ex){
		std::cout << "se viene error\n";
        //Server.close_fds() //ideal to close all files when get an exception
        std::cerr << RED << ex.what() << std::endl;
        return (1);
    }
	return (0);
}
