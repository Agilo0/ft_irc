/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerAuth.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alounici <alounici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/02 11:07:28 by yanaranj          #+#    #+#             */
/*   Updated: 2026/01/10 23:34:55 by alounici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

//to authenticate the client

void Server::passAuth(Client *cli, const std::vector<std::string> &tokens)
{
	std::cout << "PASS AUTHENTIFICATION" << std::endl;
	std::string nick = cli->getNickname().empty() ? "*" : cli->getNickname();
	if (tokens.size() < 2){
		sendResponse(cli->getClientFd(), ERR_NEEDMOREPARAMS(nick, tokens[0]));
		return;
	}
//ANTES ERA ISLOGGED
	/* if (cli->isRegistered()){//este nos salta constantemente
		sendResponse(cli->getClientFd(), ERR_ALREADYREGISTERED(nick));
		return;
	} */
	std::string pass = tokens[1];
	if (tokens[1] != _pwd){
		sendResponse(cli->getClientFd(), ERR_PASSWDMISMATCH(nick));
		return;
	}
	else
		cli->setPass();
	/* if (cli->hasAll()){
		cli->setStatus(AUTHENTICATED);
		std::cout << YELLOW << "<" << cli->getClientFd() << "> " << "Authenticated"
			<< cli->getNickname() << NC << std::endl;
		sendResponse(cli->getClientFd(), RPL_WELCOME(nick, _serverName, cli->getClientIP()));
	} */
}

void Server::nickAuth(Client *cli, const std::vector<std::string> &tokens)
{
    std::cout << YELLOW << "1st status: " << NC << cli->getStatus();
	if (tokens.size() < 2){
		//we can improve this response:
		sendResponse(cli->getClientFd(), ERR_NONICKNAMEGIVEN());
		return;
	}
	std::string nick = tokens[1];
	if (!checkNick(nick)){
		//we can improve this response:
		sendResponse(cli->getClientFd(), ERR_ERRONEUSNICKNAME(nick));
		return;
	}
	if (nickTaken(nick)){
		sendResponse(cli->getClientFd(), ERR_NICKNAMEINUSE(nick));
		return;
	}
	//nick checked, we update client info and check log info
	if (!cli->hasNickname())
		cli->setFirstNick(nick);
	else{
		cli->setNewNick(nick);
		broadcastNewNick(cli);//sends a notification to channels
	}
	/* if (cli->hasAll()){
		cli->setStatus(AUTHENTICATED);
		std::cout << YELLOW << "<" << cli->getClientFd() << "> " << "Authenticated"
			<< cli->getNickname() << NC << std::endl;
		sendResponse(cli->getClientFd(), RPL_WELCOME(nick, _serverName, cli->getClientIP()));
	} */
}
void Server::userAuth(Client *cli, const std::vector<std::string> &tokens)
{
	std::string nick = cli->getNickname().empty() ? "*" : cli->getNickname();
	std::string real;
	if (tokens.size() < 5)
	{
		sendResponse(cli->getClientFd(), ERR_NEEDMOREPARAMS(nick, tokens[0]));
		return;
	}
	std::string user = tokens[1];
	if (!checkUser(user))
	{
		sendResponse(cli->getClientFd(), ERR_ERRONEUSNICKNAME(nick));
		return;
	}
	if (tokens.size() > 5)
		real = appendToks(tokens, 4);
	else
		real = tokens[4];
	if (cli->hasUsername())
	{
		sendResponse(cli->getClientFd(), ERR_ALREADYREGISTERED(nick));
		return;
	}
	cli->setUser(user);
	cli->setRealName(real);
	/* if (cli->hasAll()){
		cli->setStatus(AUTHENTICATED);
		std::cout << YELLOW << "<" << cli->getClientFd() << "> " << "Authenticated: "
			<< cli->getNickname() << NC << std::endl;
		sendResponse(cli->getClientFd(), RPL_WELCOME(nick, _serverName, cli->getClientIP()));
	} */
}


void Server::handShake(Client *cli, const std::string &command){
	std::vector<std::string> tokens = Utils::split(command, ' ');
	if (tokens.empty())
		return;
	std::string cmd = tokens[0];
	for(size_t i = 0; i < cmd.size(); ++i)
		cmd[i] = std::toupper(cmd[i]);
	if (cmd == "CAP")
		std::cout << "IS LS\n";
	else if (cmd == "PASS")
		passAuth(cli, tokens);
	else if(cmd == "NICK")
		nickAuth(cli, tokens);
	else if (cmd == "USER")
		userAuth(cli, tokens);
/* 	else if (cmd == "JOIN"){
		std::cout << "handshake join\n";
		handleJoin(cli, tokens);
	} */
	else
	//check this error, because protocol has no params!!!!!
		sendResponse(cli->getClientFd(), ERR_NOTREGISTERED((cli->getNickname())));
	if (cli->hasAll()){
		cli->setStatus(AUTHENTICATED);
		std::cout << YELLOW << "<" << cli->getClientFd() << "> " << "Authenticated: "
			<< cli->getNickname() << NC << std::endl;
		sendResponse(cli->getClientFd(), RPL_WELCOME(cli->getNickname(), _serverName, cli->getClientIP()));
	}
}