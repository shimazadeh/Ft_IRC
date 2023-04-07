#ifndef _TREE_HPP
#define _TREE_HPP

#include "ft_irc.hpp"
#include "User.hpp"
#include "Channel.hpp"

class Tree
{
    private:
        std::map<std::string, User*>      _user_to_nick;
        std::map<int, User*>              _user_to_fd;
        std::map<std::string, Channel>    _chto_channel;

    public:
        Tree() {};
        ~Tree() {};


        void    insert(const std::string &nick_name, int    &fd)
        {
            User    *user = new User(nick_name, fd);
            _user_to_nick.insert(make_pair(nick_name, user));
            _user_to_fd.insert(std::make_pair(fd, user));
        }

        void    insert(const std::string &chan_name)
        {
            Channel chan();
            _chto_channel.insert(std::make_pair(chan_name, chan));
        }

        User    *find_usr_by_nickname(std::string& nickname)
        {
            return((_user_to_nick.find(nickname))->second);
        }

        User    *find_usr_by_fd(int& fd)
        {
            return(_user_to_fd.find(fd)->second);
        }

        Channel   &find_channel(std::string& channel)
        {
            return((_chto_channel.find(channel))->second);
        }
        void    erase_user(User &user)
        {

        }

        void    erase_channel(Channel   &user);
        //modify nickname
        //find user by fd
        //remove user
        // remove channel
};

#endif
