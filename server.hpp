#include <string>

class server
{
    private:
        int _port;
        std::string _pw;
        int _fd;

    public:
        server();
        server(int port, char *pw);
        ~server();

        void init_serv();
        // int convertport(char *port);

};