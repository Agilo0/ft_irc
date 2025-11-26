/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yanaranj <yanaranj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 16:24:32 by yanaranj          #+#    #+#             */
/*   Updated: 2025/11/26 16:59:52 by yanaranj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

#include "Server.hpp"

/*	/---------------\
   |	LIBRARIES 	 |
	\---------------/
*/
#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cctype>
#include <csignal>
#include <sys/socket.h>
#include <netinet/in.h> //Internet address family
#include  <arpa/inet.h>
#include <fcntl.h>
#include <cerrno>



/*	/---------------\
   |	COLORS	 	 |
	\---------------/
*/

#define NC "\033[0m"
#define RED "\e[1;91m"
#define GREEN "\e[1;92m"
#define YELLOW "\e[1;93m"
#define BLUE "\e[1;94m"
#define TURQUOISE "\e[1;38;5;80m"
#define FUCSIA "\e[1;38;5;201m"
#define PURPLE "\e[1;38;5;135m"
#define ORANGE "\e[1;38;2;255;128;0m"


#endif