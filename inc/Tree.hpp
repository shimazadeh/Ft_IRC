#ifndef _TREE_HPP
#define _TREE_HPP

#include "ft_irc.hpp"
#include "User.hpp"
#include "Channel.hpp"

class Tree
{
    private:
        std::unordered_map<std::string, User*>   _user_to_nick;
        std::unordered_map<int, User*>           _user_to_fd;
        std::unordered_map<std::string, Channel>   _chto_channel;

    public:
        typedef std::unordered_map<std::pair<int, std::string>, User, decltype(hashfunc)>::iterator iterator;

        Tree() {};
        ~Tree() {};


        void    insert(const std::string &nick_name, int    &fd)
        {
            User    user = new User(nick_namd, fd);
            new User user(nick_name, fd);
            _user_to_nick.insert(make_pair(nick_name, &user));
            _user_to_fd.insert(make_pair(fd, &user));
        }


        void    insert(const std::string &chan_name)
        {
            Channel chan();
            _chto_channel.insert(make_pair(chan_name, chan));
        }

        void find_usr_by_n
        //modify nickname
        //find user by fd
        //find user by nickname
        //find channel by name
        //remove user
        // remove channel
};

#endif
