/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerParse.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yanaranj <yanaranj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 15:59:17 by yanaranj          #+#    #+#             */
/*   Updated: 2025/12/08 16:22:37 by yanaranj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"

Status Client::getStatus() const{
	return this->_status;
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
	std::vector <std::string> tokens = Utils::split(cmd, ' ');
	if (tokens.empty())
		return ;
	std::string command = tokens[0];

	//for to check how many commands do we have, and its implementation
	//client commands (Agi's part)
	if (command == "JOIN"){//este if pasara a ser un else, cuando sumemos la otra parte
		//handleJoin(cli, tokens)
	}
	if (command == "WHO") {
		//handleWho(cli, tokens)
	}
	//other commands...
}