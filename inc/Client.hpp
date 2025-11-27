#include <string>
#ifndef CLIENT_HPP
# define CLIENT_HPP

class Client
{
    private:
        int _clientFd;
        //std::string username;
        //std::string nickname;
        //char **channels;

    public:
        Client();
        Client(int fd);
        ~Client();
};


#endif