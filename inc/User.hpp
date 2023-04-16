#ifndef _USER_HPP
#define _USER_HPP

#include "ft_irc.hpp"
#include "Channel.hpp"

class Channel;

class User
{
    public:
        std::string                 _nickname;
        std::string                 _realname;
        std::string                 _username;
        std::string                 _wbuff;
        std::string                 _rbuff;//this needs to be char* for memmove
        int                         _regstat;//0 = next command must be pass // 1 next command NICK // 2 mus be USER// 3 next command cannot be pass nick or user
        int                         _fd;
        bool                        _opstat;
        std::vector<Channel*>       _channels;

        User(int fd):_fd(fd){}
        User(std::string nick_name, int fd): _nickname(nick_name), _fd(fd){}
        ~User();

        void    erase_me_from_allchannel(std::vector<Channel*>  &_chan)
        {
            for (int i = 0; i < _chan.size(); ++i)
                _chan[i]->erase_user(*this);
            _chan.clear();
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
