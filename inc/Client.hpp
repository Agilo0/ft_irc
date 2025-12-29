#include <string>
#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "Channel.hpp"

enum Status {
    NOT_AUTHENTICATED,
	CAP_NEGOTIATED,
    PASS_OK,
    NICK_OK,
	NICK_AGAIN,
    USER_OK,
    AUTHENTICATED
};

class Client
{
    private:
        int _clientFd;
        std::string _nickname;
		std::string _oldnick;
        std::string _username;
        std::string _realname;
		std::string _ip;//or host
		std::string _buffer;

		bool hasNick, hasUser, hasPass, logged;
		std::vector<Channel *> _channels;
        //char **channels;
		Status _status;//fist we need to verify if client has all the requirements


    public:
        Client();
        Client(int fd);
        ~Client();
		
		Status getStatus() const;
		void setStatus(Status status);

		void setClientFd(int fd);
		//void setClientInfo(std::string username, std::string nickname);//TMP!!!
		void setClientIP(const std::string &newIP);
		int getClientFd() const;//<--socket FD of the client

		std::string getUsername() const;
		std::string getNickname() const;
		std::string getClientIP() const;
		std::string &getBuff();//

		//bools
		bool isLogged() const;
		bool hasAll() const;
		bool hasPassw() const;
		bool hasNickname() const;
		bool hasUsername() const;

		//setters
		void setPass();
		void setLog();
		void setFirstNick(std::string nick);
		void setNewNick(std::string nick);
		void setRealName(std::string name);
		void setUser(std::string name);
		void setChannel(Channel *channel);

		//getter
		std::string getNick() const;
		std::string getOldnick() const;
		Channel *getChannel(unsigned int index);


		void addBuffer(const std::string &data);
		int quitChannel(std::string channel);
		std::string createMessage();
};

#endif