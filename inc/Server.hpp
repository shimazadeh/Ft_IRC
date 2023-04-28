#ifndef _SERVER_HPP
#define _SERVER_HPP

#include "ft_irc.hpp"
#include "Tree.hpp"
#include "Parser.hpp"

class Tree;
class Parser;

class Server
{
    public:
        Server(const char*  pass, const int port);
        ~Server();

        int    check_revents(int i, short &_revents);
        void    set_up();
        void    loop();
        void    serv_exit(int _exitcode);
        void    handle_client_read(size_t &i);
        void    handle_client_write(size_t &i);
        void    handle_lsocket_read();


    private:
        Tree                        _tree;
        Parser                      _par;

        std::vector<struct pollfd>  _fds;
        struct sockaddr_in6         _addr;

        const char                  *_pass;
        const int                   _port;
        int                         _ret;
        std::vector<bool>           _closscon;

        struct pollfd               _tmpfd;
        char                       _buffer[BUFFER_SIZE];
};
#endif
