#ifndef _TREE_HPP
#define _TREE_HPP

#include "ft_irc.hpp"

class Tree
{
    private:
        std::map<std::string, User*>      _user_to_nick;
        std::map<int, User*>              _user_to_fd;
        std::map<std::string, Channel>    _chto_channel;//nicknames of all channels opened

    public:
        Tree() {};
        ~Tree() {};

        std::map<std::string, Channel>  get_channel(){return (_chto_channel);}
        std::map<std::string, User*>    get_usernick(){return (_user_to_nick);}
        std::map<int, User*>            get_userfd(){return (_user_to_fd);}

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

        std::map<std::string, Channel>::iterator find_channel(std::string& channel)
        {
            return((_chto_channel.find(channel)));
        }

        void    erase_user(User &user)
        {
            _user_to_nick.erase(user._nickname);
            _user_to_fd.erase(user._fd);
            user.erase_me_from_allchannel();

            return ;
        }

        void    erase_channel(std::string   _name)
        {
            _chto_channel.erase(_name);
        }


        //modify nickname
};

#endif
