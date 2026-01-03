/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerCommands.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaja <yaja@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/02 11:18:22 by yanaranj          #+#    #+#             */
/*   Updated: 2026/01/03 12:51:06 by yaja             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::handleJoin(Client *cli, const std::vector<std::string> &tokens){
	std::cout << ORANGE << "Handel JOIN\n" << NC;
	if (tokens.size() < 2){
		std::cerr << RED << "Not Enough params for JOIN cmd" << NC << std::endl;
		return ;
	}
	std::string channelName = tokens[1];
	std::string key;
	Channel *chan = moveCreateChannel(channelName);
	
	if (tokens.size() >= 3){
		if (chan->isModeK() == false){
			std::cout << "The channel has a key validation\n";
			chan->setKey(tokens[2]);
			key = tokens[2];
		}
		else
			std::cout << "Invalid password for <" << channelName << ">\n";
	}
	if (channelName[0] != '#'){
		sendResponse(cli->getClientFd(), ERR_NOSUCHCHANNEL(cli->getNick(), channelName));//this will send the formated response, according to IRC protocol
		return ;
	}

	if (chan->isModeI() && !chan->isOperator(cli->getClientFd()) && !chan->isInvited(cli->getClientFd())){
		sendResponse(cli->getClientFd(), ERR_INVITEONLYCHAN(channelName));
		return;
	}
	if (chan->isModeK()){
		if (key.empty() || key != chan->getKey()){
			sendResponse(cli->getClientFd(), ERR_BADCHANNELKEY(/*cli->_nickname (key is tmp)*/key, channelName));
			return;
		}
	}
	if (chan->isModeL() && chan->getClients().size() >= chan->getMaxUsers()){
	sendResponse(cli->getClientFd(), ERR_CHANNELISFULL(/*cli->_nickname (key is tmp)*/key, channelName));
		return ;
	}
	//first user of the channel is the operator
	bool isFirst = chan->getClients().empty();
    if (!isFirst)//DELETE LATER
        std::cout << TURQUOISE << "You will be the Operator of the channel: " << cli->getClientFd() << NC << std::endl;
	chan->addClient(cli->getClientFd(), isFirst);
	chan->removeInvite(cli->getClientFd());//??

	/*we need to announce of the JOIN to everyone*/
	//(:!@127.0.0.1 JOIN :#new) <--- this is how we can see the announce
	sendResponse(cli->getClientFd(), RPL_JOIN(cli->getNickname(), cli->getUsername(), cli->getClientIP(), channelName));

	//for loop to send the msg to every member of the channel
	const std::set<int> &members = chan->getClients();
	for (std::set<int>::const_iterator it = members.begin(); it != members.end(); ++it){
		std::cout << TURQUOISE << "JOIN: getClientFd(): " << cli->getClientFd() << std::endl;
		if (*it != cli->getClientFd()){
			Client *currCli = getClient(*it);
			std::cout << ORANGE << "JOIN: currCli: " << currCli << std::endl;
			if (currCli)
				//vvvvv check is we are getting the propper client FD 
				sendResponse(currCli->getClientFd(), RPL_JOIN(cli->getNickname(), cli->getUsername(), cli->getClientIP(), channelName));
		}
	}
	cli->setChannel(chan);
	std::cout << RED << "Finish sending notification to clients" << std::endl;
	//HERE WE NEED THE 1ST NICKS LIST!! (CHECK WHY)
	//std::ostringstream nicks;
	/* for(std::set<int>::const_iterator it = members.begin(); it != members.end(); ++it){
		Client *m = getClient(*it);
		if (!m) continue;//if this client does not exist
		if (chan->isOperator(*it))//this int is operator	
			
	} */
}
