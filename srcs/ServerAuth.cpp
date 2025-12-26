/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerAuth.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaja <yaja@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 15:59:17 by yanaranj          #+#    #+#             */
/*   Updated: 2025/12/26 17:20:41 by yaja             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"

void Server::nickAuth(Client *cli, const std::vector<std::string> &tokens, std::string servername)
{
    std::cout << YELLOW << "1st status: " << NC << cli->getStatus();
	if (tokens.size() < 2)
	{
		sendResponse(cli->getClientFd(), ERR_NONICKNAMEGIVEN());
		return;
	}
	std::string nick = tokens[1];
	if (!checkNick(nick))
	{
		sendResponse(cli->getClientFd(), ERR_ERRONEUSNICKNAME(nick));
		return;
	}
	
	if (nickTaken(nick))
	{
		sendResponse(cli->getClientFd(), ERR_NICKNAMEINUSE(nick));
		return;
	}

	//nick checked, we update client info and check log info
	if (!cli->hasNickname())
		cli->setFirstNick(nick);
	else
	{
		cli->setNewNick(nick);
		broadcastNewNick(cli);
	}
	
	if (cli->hasAll())
	{
		cli->setLog();
		std::cout << "logged" << std::endl;
		sendResponse(cli->getClientFd(), RPL_WELCOME(nick, servername, cli->getClientIP()));
	}
	cli->setStatus(NICK_OK);
}

void Server::passAuth(Client *cli, const std::vector<std::string> &tokens, std::string servername)
{
	std::cout << "DOING PASS AUTHENTIFICATION" << std::endl;
	std::string nick = cli->getNickname().empty() ? "*" : cli->getNickname();
	if (tokens.size() < 2)
	{
		sendResponse(cli->getClientFd(), ERR_NEEDMOREPARAMS(nick, tokens[0]));
		return;
	}

	if (cli->isLogged())//is authenticated??
	{
		sendResponse(cli->getClientFd(), ERR_ALREADYREGISTERED(nick));
		return;
	}

	std::string pass = tokens[1];
	if (pass != _pwd)
	{
		sendResponse(cli->getClientFd(), ERR_PASSWDMISMATCH(nick));
		return;
	}
	else
	{
		cli->setPass();
	}

	if (cli->hasAll())
	{
		cli->setLog();
		sendResponse(cli->getClientFd(), RPL_WELCOME(nick, servername, cli->getClientIP()));
	}
	cli->setStatus(PASS_OK);
}

void Server::userAuth(Client *cli, const std::vector<std::string> &tokens, std::string servername)
{
	std::string nick = cli->getNickname().empty() ? "*" : cli->getNickname();
	std::string user = tokens[1];
	std::string real;
	if (tokens.size() < 5)
	{
		sendResponse(cli->getClientFd(), ERR_NEEDMOREPARAMS(nick, tokens[0]));
		return;
	}
	if (tokens.size() > 5)
		real = appendToks(tokens, 4);
	else
		real = tokens[4];
	if (cli->hasUsername())
	{
		sendResponse(cli->getClientFd(), ERR_ALREADYREGISTERED(cli->getUsername()));
		return;
	}
	if (!cli->hasPassw())
	{
		sendResponse(cli->getClientFd(), ERR_NOTREGISTERED());
		return;
	}
	if (checkUser(user))
	{
		cli->setUser(user);
		cli->setRealName(real);
	}
	if (cli->hasAll())
	{
		cli->setLog();
		sendResponse(cli->getClientFd(), RPL_WELCOME(nick, servername, cli->getClientIP()));
	}
	cli->setStatus(USER_OK);
}
