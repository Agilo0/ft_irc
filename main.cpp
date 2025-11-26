/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yanaranj <yanaranj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 19:58:49 by yanaranj          #+#    #+#             */
/*   Updated: 2025/11/26 16:22:16 by yanaranj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Exception.hpp"
#include "Server.hpp"

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
    if (res < 49152 || res > 65535)
        throw IncorrectPortRange();
    return (res);
}

int main(int argc, char** argv)
{
	signal(SIGINT, Server::sigHandler);
	signal(SIGQUIT, Server::sigHandler);
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
    }
	 
    Server Server;
	Server.initServer(port, argv[2]);
	return (0);
}

//manage signal ctrl c + ctrl /
//create socket() -> ok
//listen > create socket for each usr
