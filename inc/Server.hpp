/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alounici <alounici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 16:35:09 by yanaranj          #+#    #+#             */
/*   Updated: 2025/12/17 17:42:12 by alounici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <string>

class server
{
    private:
        int _port;
        std::string _pw;
        int _fd;

    public:
        server();
        server(int port, char *pw);
        ~server();

        void init_serv();
        // int convertport(char *port);

};

#endif