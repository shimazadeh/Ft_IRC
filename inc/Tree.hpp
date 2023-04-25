#ifndef _TREE_HPP
#define _TREE_HPP

#include "ft_irc.hpp"
#include "User.hpp"
#include "Channel.hpp"

class User;
class Channel;

class Tree
{
    public:
        std::map<std::string, User*>      _user_to_nick;
        std::map<int, User*>              _user_to_fd;
        std::map<std::string, Channel>    _chto_channel;
        Tree();
        ~Tree();

        std::map<std::string, Channel>  &get_channel();
        std::map<std::string, User*>    &get_usernick();
        std::map<int, User*>            &get_userfd();

        void    insert_by_fd(int    fd);
        void    insert(const std::string chan_name);
        void    insert_by_nick(std::string& nickname, User* user);
        User    *find_usr_by_nickname(std::string& nickname);
        User    *find_usr_by_fd(int& fd);

        std::map<std::string, Channel>::iterator find_channel(std::string& channel);

        void    erase_user(User &user);
        void    erase_channel(std::string   _name);
};

#endif
