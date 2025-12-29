/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Responses.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alounici <alounici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 12:37:17 by yanaranj          #+#    #+#             */
/*   Updated: 2025/12/29 20:13:24 by alounici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

// JOIN
//#define ERR_NOSUCHCHANNEL(channel) (std::string("403 ") + channel + " :No such channel")
#define ERR_CHANNELISFULL(nickname, channel) (std::string(":server_name 471 " + nickname + " " + channel + " :Cannot join channel (+l)"))
#define ERR_INVITEONLYCHAN(channel) (std::string("473 " + channel + " :Cannot join channel (+i)"))
#define ERR_BADCHANNELKEY(nickname, channel) (std::string("475 " + nickname + " " + channel + " :Cannot join channel (+k)"))
#define RPL_JOIN(nickname, username, hostname, channel) \
    (std::string(":" + nickname + "!" + username + "@" + hostname + " JOIN :" + channel))
#define RPL_NAMREPLY(servername, nickname, channel, clientlist) \
    (std::string(":" + servername + " 353 " + nickname + " = " + channel + " :" + clientlist))
#define RPL_ENDOFNAMES(nickname, channel) (std::string(":" + server.getServerName() + " 366 " + nickname + " " + channel + " :End of /NAMES list"))


//user
#define ERR_NEEDMOREPARAMS(client, command) (std::string("461 ") + (client) + " " +(command) + " :Not enough parameters")
//PRIVMSG
#define ERR_NORECIPIENT(servername) (std::string(":" + servername + " 411 :No recipient given (PRIVMSG)"))
#define ERR_NOTEXTTOSEND(servername) (std::string(":" + servername + " 412 :No text to send"))
#define ERR_CANNOTSENDTOCHAN(servername, channel) (std::string(":" + servername + " 404 " + channel + " :Cannot send to channel"))
#define ERR_NOSUCHNICK(servername, nickname) (std::string(":" + servername + " 401 " + nickname + " :No such nick/channel"))
#define RPL_PRIVMSG(sender, receiver, message) (std::string(":" + sender + " PRIVMSG " + receiver + " " + message))



// WELCOME
#define RPL_WELCOME(nickname, servername, hostname) \
   (std::string(":" + servername + " 001 " + nickname + \
   " :Welcome to the " + servername + " Network, " + nickname + "! " + hostname))
#define RPL_YOURHOST(servername, nickname) \
    (std::string(":" + servername + " 002 " + nickname + " Your host is " + servername + ", running version 1.0"))
#define RPL_CREATED(servername, time) \
    (std::string(":" + servername + " 003 This server was created " + time))
#define RPL_MYINFO(servername, nickname) \
    (std::string(":" + servername + " 004 " + nickname + " " + servername))
#endif


//PASS
#define ERR_ALREADYREGISTERED(nick) ("462 :You may not reregister" )
#define ERR_PASSWDMISMATCH(nick) ("464 :Password incorrect" )


//NICK
#define ERR_NONICKNAMEGIVEN() (std::string("431 ") + " :No nickname given" )
#define ERR_ERRONEUSNICKNAME(nick) (std::string("432 ") + nick + " :Erroneous nickname" )
#define ERR_NICKNAMEINUSE(nick) (std::string("433 ") + nick + " :Nickname is already in use" )
#define NICK_UPDATE(oldnick, nick) (":" + oldnick + " NICK " + nick)

//USER
#define ERR_NOTREGISTERED() ("451 :You have not registered")

//PART
#define ERR_NOSUCHCHANNEL(nick, channel) ("403 " + nick + channel + " :No such channel")
#define ERR_NOTONCHANNEL(nick, channel) ("442 " + nick + channel + " :You're not on that channel")
#define RPL_PART(message, channel, reason) (message + " PART " + channel + " " + reason)

//KICK
#define ERR_CHANOPRIVSNEEDED(nick, channel) ("482 " + nick + channel + " :You're not channel operator")
#define ERR_USERNOTINCHANNEL(nick, targetnick, channel) ("441 " + nick + targetnick + channel + " :They aren't on that channel")
#define RPL_KICK(message, channel, tagetNick, reason) (message + " KICK " + channel + " " + reason)

//MODE
#define ERR_UNKNOWNCOMMAND(servername, nick, command) (servername + " 421 " + nick + command + " :Unknown command")
