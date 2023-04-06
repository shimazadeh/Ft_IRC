#ifndef _USER_HPP
#define _USER_HPP

#include "ft_irc.hpp"

class User
{
    public:
        std::string _nickname;
        std::string _realname;
        std::string _username;
        std::string _wbuff;
        std::string _rbuff;
        int         _regstat;
        int         _fd;
        bool        _opstat;

        User(int fd):_fd(fd){}
        User(std::string nick_name, int fd): _nickname(nick_name), _fd(fd){}
        ~User();
       
};
#endif
