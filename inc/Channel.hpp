#ifndef _CHANNEL_HPP
#define _CHANNEL_HPP

#include "User.hpp"
#include "ft_irc.hpp"

class User;

class Channel
 {
     public:

         typedef std::vector<User*>::iterator        iterator_user;
         typedef std::vector<std::string>::iterator  iterator_string;

         Channel();
         ~Channel();

         std::string get_name();
         std::string &get_topic();
         std::vector<User*>     get_members();

         int         size();

         void    add_member(User &user);
         void    add_ban(std::string name);
         void    add_oper(User &user);


         bool    erase_user(User &user);//idk if all prototyppe has to  be changes to user*
         bool    erase_members(User &user);
         bool    erase_opers(User &user);
         bool    erase_ban(std::string _name);

         bool    is_member(std::string _name);
         bool    is_ban(std::string _name);
         bool    is_oper(std::string _name);

        std::string print_members();

        void    send_message_all_members(std::string _msg);

     private:
         std::vector<User*>          _members;
         std::vector<User*>          _opers;
         std::vector<std::string>    _ban;
         std::string                 _name;
         std::string                 _topic;
        //do we need to create a limit for the number of users for each channel
 };
#endif
