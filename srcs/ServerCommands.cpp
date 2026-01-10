/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerCommands.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alounici <alounici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/02 11:18:22 by yanaranj          #+#    #+#             */
/*   Updated: 2026/01/10 22:41:43 by alounici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

//JOIN #channel_name ?mode (still have to check this part)
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

//<PRIVMSG dest :message to send>
//<PRIVMSG {, dest} :message to send> (check if this is correct)
void Server::handlePrivmsg(Client *cli, const std::vector<std::string> &tokens){
	if (!cli)
		return ;
	std::string origin;

	if (cli->getNickname().empty())
		origin = "*";
	else
		cli->getNickname();
	//send a msg if we don't have all the params for this command
	if (tokens.size() < 3){
		sendResponse(cli->getClientFd(), ERR_NEEDMOREPARAMS(cli->getNickname(), tokens[0]));
		return ;
	}
	std::string msg = tokens[2];
	if (tokens[2][0] == ':'){
		msg= tokens[2].substr(1);
	}
	//loop for join again the tokens, but only since the msg begins
	for (size_t i = 3; i < tokens.size(); ++i)
		msg += " " + tokens[i];

	//split the targets of the msg (by commas)
	std::vector<std::string>targets = Utils::split(tokens[1], ',');
	for(size_t t = 0; t < targets.size(); ++t){
		std::string dest = targets[t];
		if (dest.empty())
			continue;
		//multiple dests
		if (dest[0] == '#'){
			Channel *chan = NULL;
			for (size_t i = 0; i < _channels.size(); ++i){
				if (_channels[i].getName() == dest){
					chan = &_channels[i];
					break ;
				}
			}
			//is chan does not exist:
			if (!chan){
				sendResponse(cli->getClientFd(), ERR_NOSUCHCHANNEL(cli->getNickname()));
				continue;
			}
			//if the client is not part of the channel:
			if (!chan->isMember(cli->getClientFd())){
                //This error is from part?? Check the macro
				sendResponse(cli->getClientFd(), ERR_NOTONCHANNEL(cli->getNickname(), dest));
				continue;
			}
			const std::set<int>& users = chan->getClients();
			for(std::set<int>::const_iterator it = users.begin(); it != users.end(); ++it){
				if (*it != cli->getClientFd()){
					Client *other = getClient(*it);
					if (other)
						sendResponse(other->getClientFd(), RPL_PRIVMSG(cli->getNickname(), dest, msg));
				}
			}			
		}
		//single dest
		else{
			Client *targetCli = getClientByNick(dest);
			if (!targetCli){
				sendResponse(cli->getClientFd(), ERR_NOSUCHNICK(cli->getNickname(), dest));
				continue;
			}
			else
				sendResponse(targetCli->getClientFd(), RPL_PRIVMSG(cli->getNickname(), dest, msg));
		}
	}
}

void Server::handlePart(Client *cli, const std::vector<std::string> &tokens){
	if (tokens.size() < 2 || !checkSyn(tokens[1]))
	{
		sendResponse(cli->getClientFd(), ERR_NEEDMOREPARAMS(cli->getNickname(), tokens[1]));
		return;
	}
	std::vector<std::string> channels = Utils::split(tokens[1], ',');
	std::string reason;
	if (tokens.size() > 2)
		reason = appendToks(tokens, 2);
	else
		reason = "";
	std::vector<std::string>::iterator it = channels.begin();
	while (it != channels.end()){
		if (channelExist((*it)) == true){
			Channel *chan = findChannel((*it));
			if (!chan->isMember(cli->getClientFd())){
				sendResponse(cli->getClientFd(), ERR_NOTONCHANNEL(cli->getNickname(), (*it)));
				it++;
				continue;
			}
			std::string message = cli->createMessage();
			sendResponse(cli->getClientFd(), RPL_PART(message, (*it), reason));
			broadcastPart((*it), message, reason);
			chan->removeClient(cli->getClientFd());
			if (chan->isEmpty())
				removeChannel((*it));
		}
		else
			sendResponse(cli->getClientFd(), ERR_NOSUCHCHANNEL(*it));
		it++;
	}
}

void Server::handleKick(Client *cli, std::vector<std::string> &tokens){
	int targetFd = 0;
	std::string reason;
	if (tokens.size() > 2)
		reason = appendToks(tokens, 3);
	else
		reason = "";
	if (!checkKick(tokens)){
		sendResponse(cli->getClientFd(), ERR_NEEDMOREPARAMS(cli->getNickname(), tokens[0]));
		return;
	}
	if (!channelExist(tokens[1])){
		sendResponse(cli->getClientFd(), ERR_NOSUCHCHANNEL(tokens[1]));
		return;
	}
	if (!emitorInChannel(cli->getClientFd(), tokens[1])){
		sendResponse(cli->getClientFd(), ERR_NOTONCHANNEL(cli->getNickname(), tokens[1]));
		return;
	}
	if (!emitorOperator(cli->getClientFd(), tokens[1])){
		sendResponse(cli->getClientFd(), ERR_CHANOPRIVSNEEDED(cli->getNickname(), tokens[1]));
		return;
	}
	if (!(targetFd = targetExist(tokens[2]))){
		sendResponse(cli->getClientFd(), ERR_NOSUCHNICK(cli->getNickname(), tokens[2]));
		return;
	}
	if (!targetInChannel(tokens[1], targetFd)){
		sendResponse(cli->getClientFd(), ERR_USERNOTINCHANNEL(cli->getNickname(), tokens[2], tokens[1]));
		return;
	}
	removeTarget(tokens[1], targetFd);
	std::string message = cli->createMessage();
	sendResponse(targetFd, KICK(message, tokens[1], tokens[2], reason));
	broadcastKick(tokens[1], message, tokens[2], reason);
}

void Server::handleMode(Client *cli, std::vector<std::string> &tokens){
	if (tokens.size() < 3){
		sendResponse(cli->getClientFd(), ERR_NEEDMOREPARAMS(cli->getNickname(), tokens[0]));
		return;
	}
	if (tokens[1][0] != '#'){
		sendResponse(cli->getClientFd(), ERR_UNKNOWNCOMMAND(_serverName, cli->getNickname(), tokens[0]));
		return;
	}
	if (!channelExist(tokens[1])){
		sendResponse(cli->getClientFd(), ERR_NOSUCHCHANNEL(tokens[1]));
		return;
	}
	Channel *channel = findChannel(tokens[1]);
	if (!emitorInChannel(cli->getClientFd(), tokens[1])){
		sendResponse(cli->getClientFd(), ERR_NOTONCHANNEL(cli->getNickname(), tokens[1]));
		return;
	}
	if (isChangeMode(tokens[2]) && validMode(tokens[2])){
		if (channel->isOperator(cli->getClientFd())){
				int success = 0;
			int targetFd = -1;
			std::string arg = "";

			if (tokens.size() > 3){
				arg = tokens[3];
				targetFd = findTarget(arg);
				if (targetFd == -1)
				{
					sendResponse(cli->getClientFd(), ERR_NOSUCHNICK(_serverName, tokens[3]));
					return;
				}
			}
			success = channel->manageModeChange(tokens[2], arg, targetFd);
			if (success == 1){
				sendResponse(cli->getClientFd(), ERR_NEEDMOREPARAMS(cli->getNickname(), tokens[0]));
				return;
			}
			else if (success == 2){
				sendResponse(cli->getClientFd(), ERR_USERNOTINCHANNEL(cli->getNickname(), tokens[3], channel->getName()));
				return;
			}
			else if (success == 3){
				sendResponse(cli->getClientFd(), ERR_INVALIDMODEPARAM(_serverName, cli->getNickname(), channel->getName(), "l", tokens[3]));
				return;
			}
			std::string message = cli->createMessage();
			message.append(appendToks(tokens, 0));
			broadcastMode(channel, message);
		}
		else{
			sendResponse(cli->getClientFd(), ERR_CHANOPRIVSNEEDED(cli->getNickname(), channel->getName()));
			return;
		}
	}
}

void Server::handleInvite(Client *cli, std::vector<std::string> &tokens){
	if (tokens.size() < 3){
		sendResponse(cli->getClientFd(), ERR_NEEDMOREPARAMS(cli->getNickname(), tokens[0]));
		return;
	}
	if (!channelExist(tokens[2])){
		sendResponse(cli->getClientFd(), ERR_NOSUCHCHANNEL(tokens[1]));
		return;
	}
	
	if (!emitorInChannel(cli->getClientFd(), tokens[2])){
		sendResponse(cli->getClientFd(), ERR_NOTONCHANNEL(cli->getNickname(), tokens[1]));
		return;
	}
	Channel *channel = findChannel(tokens[2]);
	if (!channel->isOperator(cli->getClientFd())){
		sendResponse(cli->getClientFd(), ERR_CHANOPRIVSNEEDED(cli->getNickname(), channel->getName()));
		return;
	}
	int targetFd = findTarget(tokens[1]);
	if (targetFd == -1){
		sendResponse(cli->getClientFd(), ERR_NOSUCHNICK(_serverName, tokens[2]));
		return;
	}
	if (channel->isMember(targetFd)){
		sendResponse(cli->getClientFd(), ERR_USERONCHANNEL(_serverName, cli->getNickname(), tokens[1], channel->getName()));
		return;
	}
	channel->inviteClient(targetFd);
	sendResponse(cli->getClientFd(), RPL_INVITING(_serverName, cli->getNickname(), tokens[1], channel->getName()));
	std::string message = cli->createMessage();
	sendResponse(targetFd, INVITE(message, tokens[1], channel->getName()));
}

void Server::handleQuit(Client *cli, std::vector<std::string> &tokens)
{
	(void)cli;
	(void)tokens;
	std::string message = cli->createMessage();
	std::string reason;

	if (tokens.size() > 1)
		reason = appendToks(tokens, 1);
	else
		reason = "Client Quit";
	broadcastQuit(cli, message, reason);
	cli->markForRevome();
}

void Server::handleTopic(Client *cli, std::vector<std::string> &tokens)
{
	if (tokens.size() < 2)
	{
		sendResponse(cli->getClientFd(), ERR_NEEDMOREPARAMS(cli->getNickname(), tokens[0]));
		return;
	}
	if (!channelExist(tokens[1]))
	{
		sendResponse(cli->getClientFd(), ERR_NOSUCHCHANNEL(tokens[1]));
		return;
	}
	Channel *channel = findChannel(tokens[1]);
	if (!emitorInChannel(cli->getClientFd(), tokens[1]))
	{
		sendResponse(cli->getClientFd(), ERR_NOTONCHANNEL(cli->getNickname(), tokens[1]));
		return;
	}
	if (tokens.size() > 2)
	{
		if (channel->isModeT() && !channel->isOperator(cli->getClientFd()))
		{
			sendResponse(cli->getClientFd(), ERR_CHANOPRIVSNEEDED(cli->getNickname(), tokens[1]));
			return;
		}
		std::string topic = appendToks(tokens, 2);
		channel->setTopic(topic);
		std::string message = cli->createMessage();
		broadcastTopic(message, channel, topic);
	}
	else
	{
		if (channel->hasTopic())
			sendResponse(cli->getClientFd(), RPL_TOPIC(cli->getNickname(), channel->getName(), channel->getTopic()));
		else
			sendResponse(cli->getClientFd(), RPL_NOTOPIC(cli->getNickname(), channel->getName()));
	}
}


void Server::handleWho(Client *cli, std::vector<std::string> &tokens)
{
	if (tokens.size() < 2){
		sendResponse(cli->getClientFd(), ERR_NEEDMOREPARAMS(cli->getNickname(), tokens[0]));
		return;
	}
	int type = whoType(tokens[1]);
	if (type == 1){
		if (!channelExist(tokens[1])){
			sendResponse(cli->getClientFd(), ERR_NOSUCHCHANNEL(tokens[1]));
			return;
		}
		Channel *channel = findChannel(tokens[1]);
		if (!channel->isMember(cli->getClientFd())){
			sendResponse(cli->getClientFd(), ERR_NOTONCHANNEL(cli->getNickname(), tokens[1]));
			return;
		}
		std::set<int> clients = channel->getClients();
		std::set<int>::iterator it = clients.begin();
		while (it != clients.end()){
			std::string message = buildWhoMessage((*it), channel->isOperator(*it));
			if (!message.empty())
				sendResponse(cli->getClientFd(), RPL_WHOREPLY(cli->getNickname(), channel->getName(), message));
			it++;
		}
		sendResponse(cli->getClientFd(), RPL_ENDOFWHO(cli->getNickname(), tokens[1]));
	}
	else if (type == 2){
		int targetFd = findTarget(tokens[1]);
		if (targetFd < 0){
			sendResponse(cli->getClientFd(),
				ERR_NOSUCHNICK(cli->getNickname(), tokens[1]));
			return;
		}
		std::vector<Channel>::iterator it = _channels.begin();
		while (it != _channels.end()){
			if (it->isMember(targetFd) && it->isMember(cli->getClientFd())){
				std::string message = buildWhoMessage(targetFd, it->isOperator(targetFd));
				sendResponse(cli->getClientFd(), RPL_WHOREPLY(cli->getNickname(), it->getName(), message));
			}
			++it;
		}
		sendResponse(cli->getClientFd(), RPL_ENDOFWHO(cli->getNickname(), tokens[1]));
	}
}

void Server::handlePing(Client *cli, std::vector<std::string> &tokens){
	if (tokens.size() < 2)
		return;
	sendResponse(cli->getClientFd(), "PONG " + tokens[1]);
}
