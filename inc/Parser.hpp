#ifndef _PARSER_HPP
#define _PARSER_HPP

#include "ft_irc.hpp"

class Parser
{
    public:
        Parser(Tree &tree)
        {
            _tree = &tree;
            _cmd = "";
            _param = NULL;
            _user = NULL;
        }

        ~Parser();

        void    check_for_cmd();
        void    tokenizer();
        void    execute()
        {
            if (_cmd.compare("PRIVMSG") == 0)
                privmsg();
            else if (_cmd.compare("JOIN") == 0)
                join();
        }

        //connection operation
        void    authenticate()
        {
        }
        void    pass()
        {
            if (!_cmd.compare(_pass))
                //they can go on with authentication
            else 
                //exception wrong password
        }
        void    nick()
        {
            _user->_nickname = _param[0];
        }

        void    user()
        {
            _user->_username = _param[0];
            _user->_realname = _param[3];
        }

        void    ping(){
        }
        void    pong(){}
        void    oper(){}
        void    quit(){}
        void    error(std::string  _error)
        {
            //send the _error to the client
        }
        //channel operarion
        void    part()
        {
        }

        void    topic()
        {

        }
        void    names()
        {

        }

        void    list()
        {}

        void    invite()
        {}

        void    kick()
        {}
        //in the commands you will mostly use find, erase, insert functions of map to execute
        void    join()
        {
            std::map<std::string, Channel>::iterator it = _tree->get_channel().find(_param[0]);
            if (it != _tree->get_channel().end()) 
            {
                if (!it->second.is_ban(_user->_nickname))
                {
                    //discuss what we need to do with the keys
                    it->second.add_member(*_user);//??ask aguillar
                    //1.send a JOIN message to the client
                    if (it->second.get_topic() != "")
                        //send the channel topic to the client
                    //3.call the name command : name();
                }
                else
                    //user is banned exception
            }
            else
                //errror exception channel doesnt exist

        }
        void    privmsg()
        {
            if (_tree->get_channel().find(_param[0]) != _tree->get_channel().end()) 
            {
                //send msg to channel //check if the sender/user has permission to write in THAT channel, if not ERROR CANNOT SENDTO CHAN
            }   
            else if (_param[0].compare(_user->_nickname) == 0)
            {
                //send msg to the user

            }
            else
            {
                //error for NO chan and NO user
                //exception
            }
        }
        void    notice();


    private:
        std::string     _cmd;
        std::string     _param[3];
        User            *_user;
        Tree            *_tree;
        std::string     _pass;//discuss later




};
#endif
