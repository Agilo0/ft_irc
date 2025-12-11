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
        std::string _username;

        //char **channels;
		//Status _status;


    public:
        Client();
        ~Client();
		
		//Status getStatus() const;

		void setClientFd(int fd);
		void setUsername(std::string username);
		int getClientFd() const;//<--socket FD of the client
		std::string getUsername() const;
};


#endif