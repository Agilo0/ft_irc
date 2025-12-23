/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerCommands.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alounici <alounici@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 15:59:17 by yanaranj          #+#    #+#             */
/*   Updated: 2025/12/23 19:08:51 by alounici         ###   ########.fr       */
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
		sendResponse(cli->getClientFd(), ERR_NOSUCHCHANNEL(channelName));//this will send the formated response, according to IRC protocol
		return ;
	}

	if (chan->isModeI() && !chan->isOperator(cli->getClientFd()) && !chan->isInvited(cli->getClientFd())){
		sendResponse(cli->getClientFd(), ERR_INVITEONLYCHAN(channelName));
		return;
	}
	if (chan->isModeK()){
		if (key.empty() || key != chan->getKey()){
			std::cout << "flag_4\n";
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
	std::cout << YELLOW << "Finish sending notification to clients" << std::endl;
	//HERE WE NEED THE 1ST NICKS LIST!! (CHECK WHY)
	//std::ostringstream nicks;
	/* for(std::set<int>::const_iterator it = members.begin(); it != members.end(); ++it){
		Client *m = getClient(*it);
		if (!m) continue;//if this client does not exist
		if (chan->isOperator(*it))//this int is operator	
			
	} */
}

void Server::nickAuth(Client *cli, const std::vector<std::string> &tokens, std::string servername)
{

	if (tokens.size() < 2)
	{
		sendResponse(cli->getClientFd(), ERR_NONICKNAMEGIVEN());
		return;
	}
	std::string nick = tokens[1];
	if (!checkNick(nick))
	{
		sendResponse(cli->getClientFd(), ERR_ERRONEUSNICKNAME(nick));
		return;
	}
	
	if (nickTaken(nick))
	{
		sendResponse(cli->getClientFd(), ERR_NICKNAMEINUSE(nick));
		return;
	}

	//nick checked, we update client info and check log info
	if (!cli->hasNickname())
		cli->setFirstNick(nick);
	else
	{
		cli->setNewNick(nick);
		broadcastNewNick(cli);
	}
	
	if (cli->hasAll())
	{
		cli->setLog();
	std::cout << "logged" << std::endl;

		sendResponse(cli->getClientFd(), RPL_WELCOME(nick, servername, cli->getClientIP()));
	}

}

void Server::passAuth(Client *cli, const std::vector<std::string> &tokens, std::string servername)
{
	std::string nick = cli->getNickname().empty() ? "*" : cli->getNickname();
	if (tokens.size() < 2)
	{
		sendResponse(cli->getClientFd(), ERR_NEEDMOREPARAMS(nick, tokens[0]));
		return;
	}

	if (cli->isLogged())
	{
		sendResponse(cli->getClientFd(), ERR_ALREADYREGISTERED(nick));
		return;
	}

	std::string pass = tokens[1];
	if (pass != _pwd)
	{
		sendResponse(cli->getClientFd(), ERR_PASSWDMISMATCH(nick));
		return;
	}
	else
	{
		cli->setPass();
	}

	if (cli->hasAll())
	{
		cli->setLog();
		sendResponse(cli->getClientFd(), RPL_WELCOME(nick, servername, cli->getClientIP()));
	}

}

void Server::userAuth(Client *cli, const std::vector<std::string> &tokens, std::string servername)
{
	std::string nick = cli->getNickname().empty() ? "*" : cli->getNickname();
	std::string user = tokens[1];
	std::string real;
	if (tokens.size() < 5)
	{
		sendResponse(cli->getClientFd(), ERR_NEEDMOREPARAMS(nick, tokens[0]));
		return;
	}
	if (tokens.size() > 5)
		real = appendToks(tokens);
	else
		real = tokens[4];
	if (cli->hasUsername())
	{
		sendResponse(cli->getClientFd(), ERR_ALREADYREGISTERED(cli->getUsername()));
		return;
	}
	if (!cli->hasPassw())
	{
		sendResponse(cli->getClientFd(), ERR_NOTREGISTERED());
		return;
	}
	if (checkUser(user))
	{
		cli->setUser(user);
		cli->setRealName(real);
	}
	if (cli->hasAll())
	{
		cli->setLog();
		sendResponse(cli->getClientFd(), RPL_WELCOME(nick, servername, cli->getClientIP()));
	}
}
//<PRIVMSG dest :message to send>
//<PRIVMSG {, dest} :message to send> (check if this is correct)
void Server::handlePrivmsg(Client *cli, const std::vector<std::string> &tokens){
	if (!cli)
		return ;
	std::string origin;

	
	//IMP!!!!!!!!!!!!!! As a tmp mesurement, we'll use the client IP
	if (cli->getNickname().empty())
		origin = "*";
	else
		cli->getClientIP();
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
	//dist is a single user:
	std::string target = tokens[1];
	if (target == cli->getClientIP()){
		sendResponse(cli->getClientFd(), RPL_PRIVMSG(tokens[0], target, msg));
	}
	else
		std::cout << ">This client does not exist" << std::endl;
}