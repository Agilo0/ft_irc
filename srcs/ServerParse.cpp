/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerParse.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yanaranj <yanaranj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 15:59:17 by yanaranj          #+#    #+#             */
/*   Updated: 2025/12/09 12:54:12 by yanaranj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Responses.hpp"

Status Client::getStatus() const{
	return this->_status;
}

void Server::handleJoin(Client *cli, const std::vector<std::string> &tokens){
	if (tokens.size() < 2){
		std::cerr << RED << "Not Enough params for JOIN cmd" << NC << std::endl;
	}
	std::string channelName = tokens[1];
	if (tokens.size() >= 3){
		std::string key = tokens[2];
	}
	if (channelName[0] != '#'){
		//sendResponse();//this will send the formated response, according to IRC protocol
		std::cerr << RED << "ircserv ERROR!! Ilegal name: " << channelName << "\r\n" << NC << std::endl;
		return ;
	}
	
	
}

void Server::parseCommand(Client *cli, const std::string &cmd)
{
	if (cmd.empty())
		return ;
	if (cli->getStatus() != AUTHENTICATED){
		//handle authentication client commands
		return ;
	}
	//handle other commands
	std::vector <std::string> tokens = split(cmd, ' ');
	if (tokens.empty())
		return ;
	std::string command = tokens[0];//we want the 1st word of the line

	//for to check how many commands do we have, and its implementation
	//client commands (Agi's part)
	if (command == "JOIN"){//este if pasara a ser un else, cuando sumemos la otra parte
		std::cout << ORANGE << "Sended tokens: \n\t" << cmd << NC << std::endl;
		handleJoin(cli, tokens);
	}
	else if (command == "WHO") {
		//handleWho(cli, tokens)
	}
	else if(command == "PRIVMSG"){
		//handlePrivmsg(cli, tokens)
	}
	else if(command == "INVITE"){
		//handleInvite(cli, tokens)
	}
	else if(command == "KICK"){
		//handleKick(cli, tokens)
	}
	else if(command == "TOPIC"){
		//handleTopic(cli, tokens)
	}
	else if(command == "MODE"){
		//handleMode(cli, tokens)
	}
	else if (command == "QUIT"){
		//clear all the client fds
	}
	else{
		std::cerr << RED << "Unknown command for IRC" << NC << std::endl;
	}
}