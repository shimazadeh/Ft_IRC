#ifndef _CHANNEL_HPP
#define _CHANNEL_HPP

#include "ft_irc.hpp"

class Channel
 {
     public:

         typedef std::vector<User*>::iterator        iterator_user;
         typedef std::vector<std::string>::iterator  iterator_string;

         Channel();
         ~Channel();

         std::string get_name(){return (_name);}
         std::string &get_topic(){return (_topic);}
         int         size(){return (_members.size());}

         void    add_member(User &user)
         {
             _members.push_back(&user);
         }

         void    add_ban(std::string name)
         {
             _ban.push_back(name);
         }

         void    add_oper(User &user)
         {
             _opers.push_back(&user);
         }


         bool    erase_user(User &user)//idk if all prototyppe has to  be changes to user*
         {
            if (erase_members(user) || erase_opers(user) || erase_ban(user._nickname))
                return true;
            return false;
         }

         bool    erase_members(User &user)
         {
            for (iterator_user i = _members.begin(); i < _members.end(); i++)
            {
                if (*i == &user)
                {
                    _members.erase(i);
                    return true;
                }
            }
             return false;
         }

         bool    erase_opers(User &user)
         {
            for (iterator_user i = _opers.begin(); i < _opers.end(); i++)
            {
                if (*i == &user)
                {
                    _opers.erase(i);
                    return true;
                }
            }
             return false;
         }

         bool    erase_ban(std::string _name)
         {
            for (iterator_string i = _ban.begin(); i < _ban.end(); i++)
            {
                if ((*i).compare(_name))
                {
                    _ban.erase(i);
                    return true;
                }
            }
            return false;
         }

         bool    is_member(std::string _name)
         {
             for (iterator_user i = _members.begin(); i != _members.end(); i++)
             {
                 if (!(*i)->_nickname.compare(_name))
                     return true;
             }
             return false;
         }

         bool    is_ban(std::string _name)
         {
             for (iterator_string i = _ban.begin(); i != _ban.end(); i++)
             {
                 if (!(*i).compare(_name))
                     return true;
             }
             return false;
         }

         bool    is_oper(std::string _name)
         {
             for (iterator_user i = _opers.begin(); i != _opers.end(); i++)
             {
                 if (!(*i)->_nickname.compare(_name))
                     return true;
             }
             return false;
         }

        void    send_message_all_members(std::string _msg)
        {
            for (iterator_user it = _members.begin(); it != _members.end(); it++)
                ((*it)->_wbuff).append(_msg);
        }

     private:
         std::vector<User*>          _members;
         std::vector<User*>          _opers;
         std::vector<std::string>    _ban;
         std::string                 _name;
         std::string                 _topic;
        //do we need to create a limit for the number of users for each channel
 };
#endif
