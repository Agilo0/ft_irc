/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerCommands.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yanaranj <yanaranj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 15:59:17 by yanaranj          #+#    #+#             */
/*   Updated: 2025/12/11 12:43:06 by yanaranj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::handleJoin(Client *cli, const std::vector<std::string> &tokens){
	if (tokens.size() < 2){
		std::cerr << RED << "Not Enough params for JOIN cmd" << NC << std::endl;
	}
	std::string channelName = tokens[1];
	std::string key;
	if (tokens.size() >= 3){
		key = tokens[2];
	}
	if (channelName[0] != '#'){
		sendResponse(cli->getClientFd(), ERR_NOSUCHCHANNEL(channelName));//this will send the formated response, according to IRC protocol
		return ;
	}
	
	Channel *chan = moveCreateChannel(channelName);

	if (chan->isModeI() && !chan->isOperator(cli->getClientFd()) && !chan->isInvited(cli->getClientFd())){
		sendResponse(cli->getClientFd(), ERR_INVITEONLYCHAN(channelName));
		return;
	}
	if (chan->isModeK()){
		if (key.empty() || key != chan->getKey())
			sendResponse(cli->getClientFd(), ERR_BADCHANNELKEY(/*cli->_nickname (key is tmp)*/key, channelName));
			return;
	}
	if (chan->isModeL() && chan->getClients().size() >= chan->getMaxUsers()){
		sendResponse(cli->getClientFd(), ERR_CHANNELISFULL(/*cli->_nickname (key is tmp)*/key, channelName));
		return ;
	}
	
	//first user of the channel is the operator
	bool isFirst = chan->getClients().empty();
	chan->addClient(cli->getClientFd(), isFirst);
	chan->removeInvite(cli->getClientFd());//??

	/*we need to announce of the JOIN to everyone*/
	//sendResponse(announciong msg);

	
	
}