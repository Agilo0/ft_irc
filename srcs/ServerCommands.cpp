/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerCommands.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yanaranj <yanaranj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 15:59:17 by yanaranj          #+#    #+#             */
/*   Updated: 2025/12/10 10:19:02 by yanaranj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

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
		sendResponse(cli->getClientFd(), ERR_NOSUCHCHANNEL(channelName));//this will send the formated response, according to IRC protocol
		return ;
	}
	
	Channel *cha = moveCreateChannel(channelName);
	
	
}