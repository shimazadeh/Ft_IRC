#ifndef _USER_HPP
#define _USER_HPP

#include "ft_irc.hpp"

class User
{
    public:
        std::string                 _nickname;
        std::string                 _realname;
        std::string                 _username;
        std::string                 _wbuff;
        std::string                 _rbuff;
        int                         _regstat;
        int                         _fd;
        bool                        _opstat;
        std::vector<Channel*>       _channels;

        User(int fd):_fd(fd){}
        User(std::string nick_name, int fd): _nickname(nick_name), _fd(fd){}
        ~User();

        void    erase_me_from_allchannel()
        {
            for (int i = 0; i < _channels.size(); ++i)
                _channels[i]->erase_user(*this);
        }

        std::string find_channel(std::string    chaname)
        {
            for (std::vector<Channel*>::iterator i = _channels.begin(); i != _channels.end(); i++)
            {
                if (!((*i)->get_name().compare(chaname)))
                    return ((*i)->get_name());
            }
        }

};
#endif
