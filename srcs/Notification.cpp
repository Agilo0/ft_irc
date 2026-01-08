#include "Server.hpp"

void Server::broadcastNewNick(Client *cli)
{
	int i = 0;
	std::vector<int> clientFdsOk;
	std::vector<int> res;
	Channel *chan;

	while ((chan = cli->getChannel(i)) != NULL)
	{
		clientFdsOk = notifChannel(chan, cli->getOldnick(), cli->getNickname(), clientFdsOk);
		i++;
	}
}
std::vector<int> Server::notifChannel(Channel *chan, std::string old, std::string nick, std::vector<int> ok){
	std::set<int> clients = chan->getClients();
	std::set<int>::iterator it = clients.begin();
	
	while (it != clients.end())
	{
		int fd = *it;
		if (std::find(ok.begin(), ok.end(), fd) == ok.end())
		{
			sendResponse(fd, NICK_UPDATE(old, nick));
			ok.push_back(fd);
		}
		it++;

	}
	return (ok);
}

void Server::broadcastPart(std::string chann, std::string message, std::string reason)
{

	std::vector<Channel>::iterator it = _channels.begin();

	while (it != _channels.end())
	{
		if (chann == (*it).getName())
		{
			std::set<int> clients = (*it).getClients();
			std::set<int>::iterator itc = clients.begin();
			while (itc != clients.end())
			{
				int fd = *itc;
				sendResponse(fd, RPL_PART(message, (*it).getName(), reason));
				itc++;
			}
			return;
		}
		it++;
	}
}