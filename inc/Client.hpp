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
        //std::string username;
        //std::string nickname;
        //char **channels;
		Status _status;


    public:
        Client();
        Client(int fd);
        ~Client();
		
		Status getStatus() const;
};


#endif