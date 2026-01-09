#include "Server.hpp"

void Server::broadcastNewNick(Client *cli){
	std::vector<int> clientFdsOk;
	std::vector<int> res;
	std::vector<std::string> channels = cli->getChannelVect();
	std::vector<std::string>::iterator it = channels.begin();

	while (it != channels.end()){
		clientFdsOk = notifChannel(findChannel((*it)), cli->getOldnick(), cli->getNickname(), clientFdsOk);
		it++;
	}
}
std::vector<int> Server::notifChannel(Channel *chan, std::string old, std::string nick, std::vector<int> ok){
	std::set<int> clients = chan->getClients();
	std::set<int>::iterator it = clients.begin();
	
	while (it != clients.end()){
		int fd = *it;
		if (std::find(ok.begin(), ok.end(), fd) == ok.end()){
			sendResponse(fd, NICK_UPDATE(old, nick));
			ok.push_back(fd);
		}
		it++;

	}
	return (ok);
}

void Server::broadcastPart(std::string chann, std::string message, std::string reason){
	std::vector<Channel>::iterator it = _channels.begin();

	while (it != _channels.end()){
		if (chann == (*it).getName()){
			std::set<int> clients = (*it).getClients();
			std::set<int>::iterator itc = clients.begin();
			while (itc != clients.end()){
				int fd = *itc;
				sendResponse(fd, RPL_PART(message, (*it).getName(), reason));
				itc++;
			}
			return;
		}
		it++;
	}
}

void Server::broadcastKick(std::string chann, std::string message, std::string nick, std::string reason){
	std::vector<Channel>::iterator it = _channels.begin();

	while (it != _channels.end()){
		if (chann == (*it).getName()){
			std::set<int> clients = (*it).getClients();
			std::set<int>::iterator itc = clients.begin();
			while (itc != clients.end()){
				int fd = *itc;
				sendResponse(fd, KICK(message, chann, nick, reason));
				itc++;
			}
			return;
		}
		it++;
	}
	(void)nick;
}

void Server::broadcastMode(Channel *channel, const std::string message){
	std::set<int> clients = channel->getClients();
	std::set<int>::iterator itc = clients.begin();
	while (itc != clients.end()){
		int fd = *itc;
		sendResponse(fd, message);
		itc++;
	}
	return;
}

void Server::broadcastQuit(Client *cli, std::string message, std::string reason)
{
	std::vector<int> clientFdsOk;
    std::vector<Channel>::iterator it = _channels.begin();
    clientFdsOk.push_back(cli->getClientFd());

	while (it != _channels.end()){
        if ((*it).isMember(cli->getClientFd()))
            clientFdsOk = notifChannelQuit(&(*it), message, reason, clientFdsOk);
		it++;
	}
}

std::vector<int> Server::notifChannelQuit(Channel *chan, std::string message, std::string reason, std::vector<int> ok)
{
	std::set<int> clients = chan->getClients();
	std::set<int>::iterator it = clients.begin();
	
	while (it != clients.end())
	{
		int fd = *it;
		if (std::find(ok.begin(), ok.end(), fd) == ok.end())
		{
			sendResponse(fd, QUIT(message, reason));
			ok.push_back(fd);
		}
		it++;

	}
	return (ok);
}

void Server::broadcastTopic(std::string message, Channel *channel, std::string topic)
{

	std::set<int> clients = channel->getClients();
	std::set<int>::iterator itc = clients.begin();
	while (itc != clients.end())
	{
		int fd = *itc;
		sendResponse(fd, TOPIC(message, channel->getName(), topic));
		itc++;
	}
	return;
}