#include <string>
#ifndef CLIENT_HPP
# define CLIENT_HPP

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
		std::string _ip;
		std::string _buffer;

		bool hasNick, hasUser, hasPass, logged;	
        //char **channels;
		//Status _status;


    public:
        Client();
        Client(int fd);
        ~Client();
		
		//Status getStatus() const;

		void setClientFd(int fd);
		void setClientInfo(std::string username, std::string nickname);//TMP!!!
		void setClientIP(const std::string &newIP);
		int getClientFd() const;//<--socket FD of the client

		std::string getUsername() const;
		std::string getNickname() const;
		std::string getClientIP() const;
		std::string &getBuff();//

		bool isLogged() const;
		void setPass();
		bool hasAll() const;
		bool hasNickname() const;
		void setLog();
		void setFirstNick(std::string nick);
		void setNewNick(std::string nick);
		std::string getNick() const;
		std::string getOldnick() const;


		void addBuffer(const std::string &data);
};

#endif