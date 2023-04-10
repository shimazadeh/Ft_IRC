#ifndef _CHANNEL_HPP
#define _CHANNEL_HPP

#include "ft_irc.hpp"

class Channel
{
    public:
        Channel();
        ~Channel();

    private:
        std::vector<User*>          _members;
        std::vector<User*>          _opers;
        std::vector<std::string>    _ban;

};


#endif
