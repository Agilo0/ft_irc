/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yanaranj <yanaranj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 19:58:49 by yanaranj          #+#    #+#             */
/*   Updated: 2025/12/18 18:09:54 by yanaranj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Exception.hpp"
#include "Server.hpp"
#include "Client.hpp"

int check_port(char *port)
{
    int i = 0;
    int res = 0;

    while (port[i])
    {
        if (!isdigit(port[i]))
        {
            throw IncorrectPort();
        }
        res = res * 10 + (port[i] - '0');
        i++;
    }
    if (res != 6667)
        throw IncorrectPortRange();
    return (res);
}

int main(int argc, char** argv)
{
    std::string line;
	signal(SIGINT, Server::sigHandler);//^C. Interrupt the process
	signal(SIGQUIT, Server::sigHandler);//^\ Quit process (9/12/25->SEGV )
    
    //right now we cannot handle ^D, because is not a signal
/*    if(!std::getline(std::cin, line)){
        std::cout << "Pressing ^D\n";
        return (0);
    }*/
    int port = 0;

    if (argc != 3)
    {
        std::cerr << "Please enter port and password." << std::endl;
        return (1);
    }

    try
    {
        port = check_port(argv[1]);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return (1);
    }
	 
    Server Server;
    try{
	    Server.initServer(port, argv[2]);
    }
    catch (const std::exception &ex){
		std::cout << "se viene error\n";
        //Server.close_fds() //ideal to close all files when get an exception
        std::cerr << RED << ex.what() << std::endl;
        return (1);
    }
	return (0);
}

//manage signal ctrl c + ctrl /
//create socket() -> ok
//listen > create socket for each usr
