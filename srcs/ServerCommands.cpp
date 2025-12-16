/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerCommands.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yanaranj <yanaranj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 15:59:17 by yanaranj          #+#    #+#             */
/*   Updated: 2025/12/16 18:37:10 by yanaranj         ###   ########.fr       */
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
	sendResponse(cli->getClientFd(), RPL_JOIN(cli->getNickname(), cli->getUsername(), cli->getClientIP(), channelName));

	//for loop to send the msg to every member of the channel
	const std::set<int> &members = chan->getClients();
	for (std::set<int>::const_iterator it = members.begin(); it != members.end(); ++it){
		std::cout << TURQUOISE << "JOIN: getClientFd(): " << cli->getClientFd() << std::endl;
		if (*it != cli->getClientFd()){
			Client *currCli = getClient(*it);//this is suppose to get the client FD
			std::cout << ORANGE << "JOIN: currCli: " << currCli << std::endl;
			if (currCli)
				//vvvvv check is we are getting the propper client FD 
				sendResponse(currCli->getClientFd(), RPL_JOIN(cli->getNickname(), cli->getUsername(), cli->getClientIP(), channelName));
		}
	}

	//HERE WE NEED THE 1ST NICKS LIST!! (CHECK WHY)
	//std::ostringstream nicks;
	/* for(std::set<int>::const_iterator it = members.begin(); it != members.end(); ++it){
		Client *m = getClient(*it);
		if (!m) continue;//if this client does not exist
		if (chan->isOperator(*it))//this int is operator	
			
	} */
}