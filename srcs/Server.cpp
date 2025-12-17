
#include "../inc/Server.hpp"
#include <iostream>
#include <sys/socket.h>

void Server::checkNewData(int fd){//this will give us the commands that are sending the clients
	std::cout << YELLOW << "NEW DATA FUNCTION:" << std::endl;
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));//clears the buffer
	
	std::cout << "buffer1: " << buffer << std::endl;
	int bytes = recv(fd, buffer, sizeof(buffer), 0);
	if (bytes <= 0) //is the client disconnected?
	{
		close(fd);//should be marked TO REMOVE LATER
		//pollFds.erase(pollFds.begin());
		//_clients.erase(_clients.begin() + (i - 1));
		return ;
	}
	Client *cli = getClient(fd);
	if (!cli)
		return ;
	cli->addBuffer(std::string(buffer, bytes));
	std::string &buff = cli->getBuff();
	
	std::cout << "buffer2: " << buffer << std::endl;
	std::cout << "buffer3: " << buff << std::endl;
	size_t pos;

	//vvvvv we will process only a complete line
	//from here we'll work the commands
	while((pos = buff.find("\r\n")) != std::string::npos){
		std::string cmd = buff.substr(0, pos);
		buff.erase(0, pos + 2);
		std::cout << "buffer4: " << buff << std::endl;
		if (!cmd.empty()){
			std::cout << YELLOW << "<" << fd << ">: command " << cmd << NC << std::endl;
			parseCommand(cli, cmd);
		}
	}
	
}

