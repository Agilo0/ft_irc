/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alounici <alounici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 19:58:49 by yanaranj          #+#    #+#             */
/*   Updated: 2025/12/17 17:42:53 by alounici         ###   ########.fr       */
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
    server serv(port, argv[2]);
    // std::cout << port << std::endl;
    return (0);
}

//manage signal ctrl c + ctrl /
//create socket() -> ok
//listen > create socket for each usr
