/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerCommands.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaja <yaja@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/02 11:18:22 by yanaranj          #+#    #+#             */
/*   Updated: 2026/01/04 19:38:50 by yaja             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::handleJoin(Client *cli, const std::vector<std::string> &tokens){
	std::cout << ORANGE << "Handel JOIN\n" << NC;
    //seguridad de registro?
	if (tokens.size() < 2){
        sendResponse(cli->getClientFd(), ERR_NEEDMOREPARAMS(cli->getNickname(), "JOIN"));
		return ;
	}
	std::string channelName = tokens[1];
	std::string key;
	
	if (channelName.empty() || channelName[0] != '#'){
		sendResponse(cli->getClientFd(), ERR_NOSUCHCHANNEL(channelName));
		return ;
	}
    if (tokens.size() >= 3){
			//chan->setKey(tokens[2]);
			key = tokens[2];
	}
	Channel *chan = moveCreateChannel(channelName);
	if (chan->isModeI() && !chan->isOperator(cli->getClientFd()) && !chan->isInvited(cli->getClientFd())){
		sendResponse(cli->getClientFd(), ERR_INVITEONLYCHAN(channelName));
		return;
	}
	if (chan->isModeK()){
		if (key.empty() || key != chan->getKey()){
			sendResponse(cli->getClientFd(), ERR_BADCHANNELKEY(cli->getNickname(), channelName));
			return;
		}
	}
	if (chan->isModeL() && chan->getClients().size() >= chan->getMaxUsers()){
	sendResponse(cli->getClientFd(), ERR_CHANNELISFULL(cli->getNickname(), channelName));
		return ;
	}
	//first user of the channel is the operator
	bool isFirst = chan->getClients().empty();
	chan->addClient(cli->getClientFd(), isFirst);
	chan->removeInvite(cli->getClientFd());
	cli->setChannel(chan);
    

	/*we need to announce of the JOIN to everyone*/
	//(:!@127.0.0.1 JOIN :#new) <--- this is how we can see the announce
	sendResponse(cli->getClientFd(), RPL_JOIN(cli->getNickname(), cli->getUsername(), cli->getClientIP(), channelName));

	//for loop to send the msg to every member of the channel
	const std::set<int> &members = chan->getClients();
	for (std::set<int>::const_iterator it = members.begin(); it != members.end(); ++it){
		if (*it != cli->getClientFd()){
			Client *currCli = getClient(*it);
			std::cout << ORANGE << "JOIN: sending msg to: " << cli->getNickname() << std::endl;
			if (currCli)
				//vvvvv check is we are getting the propper client FD 
				sendResponse(currCli->getClientFd(), RPL_JOIN(cli->getNickname(), cli->getUsername(), cli->getClientIP(), channelName));
		}
	}
    // Keep track of channels this client belongs to (needed for PART, QUIT, NICK broadcasts)
	std::string nickList;
	for(std::set<int>::const_iterator it = members.begin(); it != members.end(); ++it){
		Client *m = getClient(*it);
		if (!m)
            continue;//if this client does not exist
		if (chan->isOperator(*it))
            nickList += "@";
        nickList += m->getNickname() + " ";
	}
    if (!nickList.empty())
        nickList.erase(nickList.size() - 1);
    sendResponse(cli->getClientFd(), RPL_NAMREPLY(_serverName, cli->getNickname(), channelName, nickList));
    sendResponse(cli->getClientFd(), RPL_ENDOFNAMES(_serverName, cli->getNickname(), channelName));
}
