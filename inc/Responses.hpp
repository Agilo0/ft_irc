/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Responses.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alounici <alounici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 12:37:17 by yanaranj          #+#    #+#             */
/*   Updated: 2025/12/19 20:24:44 by alounici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

// JOIN
#define ERR_NOSUCHCHANNEL(channel) (std::string("403 ") + channel + " :No such channel")
#define ERR_CHANNELISFULL(nickname, channel) (std::string(":server_name 471 " + nickname + " " + channel + " :Cannot join channel (+l)"))
#define ERR_INVITEONLYCHAN(channel) (std::string("473 " + channel + " :Cannot join channel (+i)"))
#define ERR_BADCHANNELKEY(nickname, channel) (std::string("475 " + nickname + " " + channel + " :Cannot join channel (+k)"))
#define RPL_JOIN(nickname, username, hostname, channel) \
    (std::string(":" + nickname + "!" + username + "@" + hostname + " JOIN :" + channel))
#define RPL_NAMREPLY(servername, nickname, channel, clientlist) \
    (std::string(":" + servername + " 353 " + nickname + " = " + channel + " :" + clientlist))
#define RPL_ENDOFNAMES(nickname, channel) (std::string(":" + server.getServerName() + " 366 " + nickname + " " + channel + " :End of /NAMES list"))


// WELCOME
// #define RPL_WELCOME(nickname, servername, hostname) \
 //   (std::string(":" + servername + " 001 " + nickname + \
  //  " :Welcome to the " + servername + " Network, " + nickname + "!" + hostname))
#define RPL_YOURHOST(servername, nickname) \
    (std::string(":" + servername + " 002 " + nickname + " Your host is " + servername + ", running version 1.0"))
#define RPL_CREATED(servername, time) \
    (std::string(":" + servername + " 003 This server was created " + time))
#define RPL_MYINFO(servername, nickname) \
    (std::string(":" + servername + " 004 " + nickname + " " + servername))
#endif


//PASS
#define ERR_NEEDMOREPARAMS(nick) (std::string("461 ") + nick + " PASS :Not enough parameters" )
#define ERR_ALREADYREGISTERED(nick) (std::string("462 ") + nick + " :You may not reregister" )
#define ERR_PASSWDMISMATCH(nick) (std::string("464 ") + nick + " :Password incorrect" )


//NICK
#define ERR_NONICKNAMEGIVEN() (std::string("431 ") + " :No nickname given" )
#define ERR_ERRONEUSNICKNAME(nick) (std::string("432 ") + nick + " :Erroneous nickname" )
#define ERR_NICKNAMEINUSE(nick) (std::string("433 ") + nick + " :Nickname is already in use" )
#define RPL_WELCOME(nick) (std::string("001") + nick + " :Welcome to the Internet Relay Network" + nick)
#define NICK_UPDATE(oldnick, nick) (":" + oldnick + " NICK " + nick)
