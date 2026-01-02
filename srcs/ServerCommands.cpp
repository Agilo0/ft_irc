/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerCommands.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alounici <alounici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 15:59:17 by yanaranj          #+#    #+#             */
/*   Updated: 2026/01/02 18:47:16 by alounici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"

//JOIN #channe_name
//JOIN #channe_name password
//JOIN #channel_name ?mode (still have to check this part)
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
			Client *currCli = getClient(*it);//this is suppose to get the client FD
			std::cout << ORANGE << "JOIN: currCli: " << currCli << std::endl;
			if (currCli)
				//vvvvv check is we are getting the propper client FD 
				sendResponse(currCli->getClientFd(), RPL_JOIN(cli->getNickname(), cli->getUsername(), cli->getClientIP(), channelName));
		}
	}
	cli->setChannel(chan);
	std::cout << YELLOW << "Finish sending notification to clients" << std::endl;
	//HERE WE NEED THE 1ST NICKS LIST!! (CHECK WHY)
	//std::ostringstream nicks;
	/* for(std::set<int>::const_iterator it = members.begin(); it != members.end(); ++it){
		Client *m = getClient(*it);
		if (!m) continue;//if this client does not exist
		if (chan->isOperator(*it))//this int is operator	
			
	} */
}


void Server::handlePart(Client *cli, const std::vector<std::string> &tokens)
{
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
	int chanstat = 0;
	while (it != channels.end())
	{
		if (channelExist((*it)) == true)
		{
			chanstat = cli->quitChannel((*it));
			if (chanstat == 1)
				sendResponse(cli->getClientFd(), ERR_NOTONCHANNEL(cli->getNickname(), (*it)));
			std::string message = cli->createMessage();
			sendResponse(cli->getClientFd(), RPL_PART(message, (*it), reason));
			broadcastPart((*it), message, reason);
			if (chanstat == 2)
				removeChannel((*it));
		}
		else
			sendResponse(cli->getClientFd(), ERR_NOSUCHCHANNEL(cli->getNickname(), (*it)));
		it++;
	}

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
				sendResponse(cli->getClientFd(), ERR_NOSUCHCHANNEL(cli->getNickname(), dest));
				continue;
			}
			//if the client is not part of the channel:
			if (!chan->isMember(cli->getClientFd())){
				sendResponse(cli->getClientFd(), ERR_NOTONCHANNEL(cli->getNickname(), dest));
				continue;
			}
			const std::set<int>& users = chan->getClients();
			for(std::set<int>::const_iterator it = users.begin(); it != users.end(); ++it){
				if (*it != cli->getClientFd()){
					Client *other = getClient(*it);
					if (other)
						sendResponse(cli->getClientFd(), RPL_PRIVMSG(cli->getNickname(), dest, msg));
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


void Server::handleKick(Client *cli, std::vector<std::string> &tokens)
{
	int targetFd = 0;
	std::string reason;
	if (tokens.size() > 2)
		reason = appendToks(tokens, 3);
	else
		reason = "";
	if (!checkKick(tokens))
	{
		sendResponse(cli->getClientFd(), ERR_NEEDMOREPARAMS(cli->getNickname(), tokens[0]));
		return;
	}
	if (!channelExist(tokens[1]))
	{
		sendResponse(cli->getClientFd(), ERR_NOSUCHCHANNEL(cli->getNickname(), tokens[1]));
		return;
	}
	if (!emitorInChannel(cli->getClientFd(), tokens[1]))
	{
		sendResponse(cli->getClientFd(), ERR_NOTONCHANNEL(cli->getNickname(), tokens[1]));
		return;
	}
	if (!emitorOperator(cli->getClientFd(), tokens[1]))
	{
		sendResponse(cli->getClientFd(), ERR_CHANOPRIVSNEEDED(cli->getNickname(), tokens[1]));
		return;
	}
	if (!(targetFd = targetExist(tokens[2])))
	{
		sendResponse(cli->getClientFd(), ERR_NOSUCHNICK(cli->getNickname(), tokens[1]));
		return;
	}
	if (!targetInChannel(tokens[1], targetFd))
	{
		sendResponse(cli->getClientFd(), ERR_USERNOTINCHANNEL(cli->getNickname(), tokens[2], tokens[1]));
		return;
	}
	removeTarget(tokens[1], targetFd);
	cli->quitChannel(tokens[1]);
	std::string message = cli->createMessage();
	sendResponse(targetFd, RPL_KICK(message, tokens[1], tokens[2], reason));
	broadcastKick(tokens[1], message, tokens[2], reason);
}

void Server::handleMode(Client *cli, std::vector<std::string> &tokens)
{
	if (tokens.size() < 3)
	{
		sendResponse(cli->getClientFd(), ERR_NEEDMOREPARAMS(cli->getNickname(), tokens[0]));
		return;
	}
	if (tokens[1][0] != '#')
	{
		sendResponse(cli->getClientFd(), ERR_UNKNOWNCOMMAND(_serverName, cli->getNickname(), tokens[0]));
		return;
	}
	if (!channelExist(tokens[1]))
	{
		sendResponse(cli->getClientFd(), ERR_NOSUCHCHANNEL(cli->getNickname(), tokens[1]));
		return;
	}
	Channel *channel = findChannel(tokens[1]);
	if (!emitorInChannel(cli->getClientFd(), tokens[1]))
	{
		sendResponse(cli->getClientFd(), ERR_NOTONCHANNEL(cli->getNickname(), tokens[1]));
		return;
	}
	if (isChangeMode(tokens[2]) && validMode(tokens[2]))
	{
		if (channel->isOperator(cli->getClientFd()))
		{
				int success = 0;
			int targetFd = -1;
			std::string arg = "";

			if (tokens.size() > 3)
			{
				arg = tokens[3];
				targetFd = findTarget(arg);
				if (targetFd == -1)
				{
					sendResponse(cli->getClientFd(), ERR_NOSUCHNICK(_serverName, tokens[3]));
					return;
				}
			}
			success = channel->manageModeChange(tokens[2], arg, targetFd);
			if (success == 1)
			{
				sendResponse(cli->getClientFd(), ERR_NEEDMOREPARAMS(cli->getNickname(), tokens[0]));
				return;
			}
			else if (success == 2)
			{
				sendResponse(cli->getClientFd(), ERR_USERNOTINCHANNEL(cli->getNickname(), tokens[3], channel->getName()));
				return;
			}
			else if (success == 3)
			{
				sendResponse(cli->getClientFd(), ERR_INVALIDMODEPARAM(_serverName, cli->getNickname(), channel->getName(), "l", tokens[3]));
				return;
			}
			std::string message = cli->createMessage();
			message.append(appendToks(tokens, 0));
			broadcastMode(channel, message);
		}
		else
		{
			sendResponse(cli->getClientFd(), ERR_CHANOPRIVSNEEDED(cli->getNickname(), channel->getName()));
			return;
		}
	}

}

void Server::handleInvite(Client *cli, std::vector<std::string> &tokens)
{
	if (tokens.size() < 3)
	{
		sendResponse(cli->getClientFd(), ERR_NEEDMOREPARAMS(cli->getNickname(), tokens[0]));
		return;
	}
	if (!channelExist(tokens[2]))
	{
		sendResponse(cli->getClientFd(), ERR_NOSUCHCHANNEL(cli->getNickname(), tokens[1]));
		return;
	}
	
	if (!emitorInChannel(cli->getClientFd(), tokens[2]))
	{
		sendResponse(cli->getClientFd(), ERR_NOTONCHANNEL(cli->getNickname(), tokens[1]));
		return;
	}
	Channel *channel = findChannel(tokens[2]);
	if (!channel->isOperator(cli->getClientFd()))
	{
		sendResponse(cli->getClientFd(), ERR_CHANOPRIVSNEEDED(cli->getNickname(), channel->getName()));
		return;
	}
	int targetFd = findTarget(tokens[1]);
	if (targetFd == -1)
	{
		sendResponse(cli->getClientFd(), ERR_NOSUCHNICK(_serverName, tokens[2]));
		return;
	}
	if (channel->isMember(targetFd))
	{
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
	std::string message = cli->createMessage();
	std::string reason;

	if (tokens.size() > 1)
		reason = appendToks(tokens, 1);
	else
		reason = "";
	broadcastQuit(cli, "this is a message", "this is a reason");
	deleteClient(cli);
}