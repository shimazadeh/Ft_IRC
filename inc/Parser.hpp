#ifndef _PARSER_HPP
#define _PARSER_HPP

#include "ft_irc.hpp"

#include "Channel.hpp"
#include "Tree.hpp"

class Tree;
class Channel;

class Parser
{
    public:
        Parser(Tree& tree, std::string password)
        {
            _tree = &tree;
            _cmd = "";
            _user = NULL;
            _pass = password;
        }

        ~Parser()
        {
        }

        void    change_user(User    *username)
        {
            _user = username;
        }

        User    *get_user(){return (_user);}

        int    check_for_cmd()
        {
            std::string     buf = _user->_rbuff;
            int             pos(0);

            pos = buf.find("\r\n");
            if (pos == std::string::npos)
                return (1);
            memmove(&(_user->_rbuff)[0], &(buf[0]), pos +1);
            buf = buf.substr(0, pos);
            fill_in_params(buf);
            return 0;
        }

        void    fill_in_params(std::string  buf)
        {
            std::string::size_type prev_pos = 0, pos = 0;

            while((pos = buf.find(32 , pos)) != std::string::npos || (pos = buf.find(9, pos)) != std::string::npos)
            {
                std::string substring( buf.substr(prev_pos, pos-prev_pos) );

                _param.push_back(substring);
                prev_pos = ++pos;
            }
            _param.push_back(buf.substr(prev_pos, pos-prev_pos));

            _cmd = *(_param.begin());
            _param.erase(_param.begin());
        }

        std::vector<std::string>    custom_split(std::string buf)
        {
           std::string::size_type       prev_pos = 0, pos = 0;
           std::vector<std::string>     res;


            while((pos = buf.find(',' , pos)) != std::string::npos)
            {
                std::string substring( buf.substr(prev_pos, pos-prev_pos) );

                res.push_back(substring);
                prev_pos = ++pos;
            }
            res.push_back(buf.substr(prev_pos, pos-prev_pos));
            return (res);
        }

        void    execute()
        {
            if (_cmd.compare("PASS") == 0)
                pass();
            else if (_cmd.compare("NICK") == 0)
                nick();
            else if (_cmd.compare("PRIVMSG") == 0)
                user();
            else if (_cmd.compare("PING") == 0)
                ping();
            else if (_cmd.compare("PONG") == 0)
                pong();
            else if (_cmd.compare("USER") == 0)
                user();
            else if (_cmd.compare("QUIT") == 0)
                quit();
            else if (_cmd.compare("PART") == 0)
                part();
            else if (_cmd.compare("TOPIC") == 0)
                topic();
            else if (_cmd.compare("INVITE") == 0)
                invite();
            else if (_cmd.compare("KICK") == 0)
                kick();
            else if (_cmd.compare("JOIN") == 0)
                join();
            else if (_cmd.compare("PRIVMSG") == 0)
                privmsg();
            else if (_cmd.compare("NOTICE") == 0)
                notice();
            else
               (_user->_wbuff).append("Error: No such command\n");
        }

        void    pass()
        {
            if (_user->_regstat > 1)
               (_user->_wbuff).append("PASS: error: user is already connected\n");
            else if (_param.size() != 1)
               (_user->_wbuff).append("PASS: wrong number of parameters\n");
            else if (!(*(_param.begin())).compare(_pass))
                _user->_regstat = 1;
            else
               (_user->_wbuff).append("PASS: error: wrong password, try again !\n");
        }

        void    nick()
        {
            if (_param.size() != 1)
               (_user->_wbuff).append("NICK: error: invalid number of parameters!\n");
            else if (_user->_regstat != 1)
               (_user->_wbuff).append("NICK: error: user is not registered\n");
            else if (_tree->get_usernick().find(*(_param.begin())) != _tree->get_usernick().end())
               (_user->_wbuff).append("NICK: error: nickname is already in use choose a different one\n");
            else if (_tree->get_channel().find(*(_param.begin())) != _tree->get_channel().end())
               (_user->_wbuff).append("NICK: error: nickname is already in the network\n");
            else
            {
                _user->_regstat = 2;
                _user->_nickname = *(_param.begin());
            }
        }

        void    user()
        {
            if (_param.size() != 4)
               (_user->_wbuff).append("USER: error; invalid number of parameters!\n");
            else if (_user->_regstat != 2)
               (_user->_wbuff).append("USER: error: user is not registered\n");
            else
            {
                _user->_regstat = 3;
                _user->_username = *(_param.begin());
                _user->_realname = *(_param.begin() + 2);
            }
        }

        void    ping()
        {
            if (_param.size() != 1)
               (_user->_wbuff).append("PING: error : invalid number of parameters!\n");
            else if (_user->_regstat != 3)
               (_user->_wbuff).append("PING: error: user is not registered\n");
            else if (*(_param.begin()) == "")
            {
               (_user->_wbuff).append("PING: error: wrong parameter\n");
            }
            else
               (_user->_wbuff).append("PING\n");
        }

        void    pong()
        {
            if (_param.size() != 1)
               (_user->_wbuff).append("PONG: error: invalid number of parameters!\n");
            else if (_user->_regstat != 3)
               (_user->_wbuff).append("PONG: error: user is not registered\n");
            else if (*(_param.begin()) == "")
            {
               (_user->_wbuff).append("PONG: error: wrong parameter\n");
            }
            else
               (_user->_wbuff).append("PONG\n");
        }

        void    oper()//incomplete ,question: what is the password and name ??
        {
            if (_param.size() != 2)
               (_user->_wbuff).append("OPER: error: invalid number of parameters!\n");
            else if (_user->_regstat != 3)
               (_user->_wbuff).append("OPER: error: user is not registered\n");
            else
            {
                //make the user admin of the channel
            }
        }

        void    quit()
        {
            if (_param.size() != 1)
               (_user->_wbuff).append("QUIT: error: invalid number of parameters!\n");
            else if (_user->_regstat != 3)
               (_user->_wbuff).append("QUIT: error: user is not registered, register first before quitting\n");
            else
            {
                std::vector<Channel*>   ch_tmp = _user->_channels;

                _user->erase_me_from_allchannel(_user->_channels);
                _tree->erase_user(*_user);
                for (int i = 0; i < ch_tmp.size(); i++)
                    ch_tmp[i]->send_message_all_members("QUIT: " + _user->_nickname + " has left the channel " + *(_param.end() - 1));
            }
        }

        void    part()
        {
            if (_param.size() < 2)
               (_user->_wbuff).append("PART: error: invalid number of parameters!\n");
            else if (_user->_regstat != 3)
               (_user->_wbuff).append("PART: error: user is not registered\n");
            else if (*(_param.begin()) == "" || *(_param.begin() + 1) == "")
               (_user->_wbuff).append("PART: error: wrong parameters\n");
            else
            {
                std::vector<std::string>    params = custom_split(*(_param.begin()));

                for (int i = 0; i < params.size(); i++)
                {
                    std::map<std::string, Channel>::iterator    it = _tree->get_channel().find(*(params.begin() + i));

                    if (it == _tree->get_channel().end())
                       (_user->_wbuff).append("PART: error: channel " + *(params.begin() + i) + " does not exist\n");
                    else if (!it->second.is_member(_user->_nickname))
                       (_user->_wbuff).append("PART: error: user is not in the channel\n");
                    else
                    {
                        it->second.erase_members(*_user);
                        it->second.send_message_all_members("PART: " + _user->_nickname + " has left the channel " + *(_param.end() - 1));
                    }
                }
            }
        }

        void    topic()
        {
            std::map<std::string, Channel>::iterator it = _tree->get_channel().find(*(_param.begin()));

            if (_param.size() < 1 || _param.size() > 2)
               (_user->_wbuff).append("TOPIC: invalid number of parameters!\n");
            else if (_user->_regstat != 3)
               (_user->_wbuff).append("TOPIC: error: user is not registered\n");
            else if (*(_param.begin()) == "")
            {
               (_user->_wbuff).append("TOPIC: error: wrong parameter\n");
            }
            else if (it != _tree->get_channel().end())
            {
                it->second.get_topic() = *(_param.begin() + 1);
                it->second.send_message_all_members("TOPIC: the topic of the channels has been changed to " + *(_param.begin() + 1));
            }
            else
               (_user->_wbuff).append("TOPIC: error: channel does not exist\n");

        }

        void    invite()
        {
            std::map<std::string, Channel>::iterator it = _tree->get_channel().find(_param[0]);

            if (_param.size() != 2)
               (_user->_wbuff).append("INVITE: invalid number of parameters!\n");
            else if (_user->_regstat != 3)
               (_user->_wbuff).append("INVITE: error: user is not registered\n");
            else if (*(_param.begin()) == "" || *(_param.begin() + 1) == "")
            {
               (_user->_wbuff).append("INVITE: error: wrong parameters\n");
            }
            else if (it != _tree->get_channel().end())
            {
                if (it->second.size() < 1)
                    (_user->_wbuff).append("INVITE: error: empty channel\n");
                else if (it->second.is_member(_param[0]))
                   (_user->_wbuff).append("INVITE: error: you are already in the channel !\n");
                else
                {
                    it->second.add_member(*(_tree->find_usr_by_nickname(*(_param.begin()))));
                   (_user->_wbuff).append(_user->_nickname + " has been successfully added to " + *(_param.begin() + 1));
                }
            }
            else
               (_user->_wbuff).append("INVITE: error: channel does not exist\n");
        }

        void    kick()
        {
            std::string channelName;
            std::string userToKick;
            std::map<std::string, Channel>::iterator it = _tree->get_channel().find(_param[0]);

            if (_param.size() < 2)//double check later
               (_user->_wbuff).append("KICK: invalid number of parameters!\n");
            else if (_user->_regstat != 3)
               (_user->_wbuff).append("KICK: error: user is not registered\n");
            else if (*(_param.begin()) == "" || *(_param.begin() + 1) == "")
            {
               (_user->_wbuff).append("KICK: error: wrong parameters\n");
            }
            else if (it == _tree->get_channel().end())
               (_user->_wbuff).append("KICK: error: channel does not exist\n");
            else if (it != _tree->get_channel().end())
            {
                if (it->second.is_member(_param[1]))
                    it->second.erase_user(*(_tree->find_usr_by_nickname(*(_param.begin() + 1))));
                else if (it->second.is_oper(*(_param.begin() + 1)))
                   (_user->_wbuff).append("User cannot be kicked because he is an administrator !");
                else
                   (_user->_wbuff).append("KICK: error: user does not exist\n");
            }
        }

        void    join()
        {
            std::map<std::string, Channel>::iterator it = _tree->get_channel().find(_param[0]);

            if (_param.size() < 1)
               (_user->_wbuff).append("JOIN: invalid number of parameters!\n");
            else if (_user->_regstat != 3)
               (_user->_wbuff).append("JOIN: error: user is not registered\n");
            else if (*(_param.begin()) == "" || *(_param.begin() + 1) == "")
            {
               (_user->_wbuff).append("JOIN: error: wrong parameters\n");
            }
            else if (it != _tree->get_channel().end())
            {
                if (!it->second.is_ban(_user->_nickname))
                {
                    //discuss what we need to do with the keys
                    it->second.add_member(*_user);
                   (_user->_wbuff).append("JOIN: you've joined the channel " + it->second.get_name());
                    if (it->second.get_topic() != "")
                       (_user->_wbuff).append("JOIN: topic of the channel is : " + it->second.get_topic());
                    //(_user->wbuff).append() send a list of users of the channel to the user
                }
                else
                   (_user->_wbuff).append("JOIN: error: channel does not exist\n");
            }
            else
               (_user->_wbuff).append("JOIN: error: " +  _user->_nickname + "is banned\n");

        }

        void    privmsg()
        {
            if (_param.size() < 2)
               (_user->_wbuff).append("PRIVMSG: invalid number of parameters!\n");
            else if (_user->_regstat != 3)
               (_user->_wbuff).append("PRIVMSG: error: user is not registered\n");
            else if (*(_param.begin()) == "" || *(_param.begin() + 1) == "")
            {
               (_user->_wbuff).append("PRIVMSG: error: wrong parameters\n");
            }
            else
            {
                std::vector<std::string>    targets = custom_split(*(_param.begin()));

                for (int i = 0; i < targets.size(); i++)
                {
                    User* tmp =_tree->find_usr_by_nickname(*(targets.begin() + i));
                    if (tmp)
                    (tmp->_wbuff).append(*(_param.begin() + 1));
                    else
                    {
                        std::map<std::string, Channel>::iterator tmp2 = _tree->find_channel(*(targets.begin() + i));
                        if (tmp2 != _tree->get_channel().end())
                            (tmp2->second.send_message_all_members(*(_param.begin() + 1)));
                        else
                            (_user->_wbuff).append("PRIVMSG: error: no such target\n");
                    }
                }
                //send msg to channel //check if the sender/user has permission to write in THAT channel, if not ERROR CANNOT SENDTO CHAN
            }
        }

        void    notice()
        {
            if (_param.size() < 2)
               (_user->_wbuff).append("NOTICE: invalid number of parameters!\n");
            else if (_user->_regstat != 3)
               (_user->_wbuff).append("NOTICE: error: user is not registered\n");
            else if (*(_param.begin()) == "" || *(_param.begin() + 1) == "")
            {
               (_user->_wbuff).append("NOTICE: error: wrong parameters\n");
            }
            else
            {
                std::vector<std::string>    targets = custom_split(*(_param.begin()));

                for (int i = 0; i < targets.size(); i++)
                {
                    User* tmp =_tree->find_usr_by_nickname(*(targets.begin() + i));
                    if (tmp)
                        (tmp->_wbuff).append(*(_param.begin() + 1));
                    else
                        (_user->_wbuff).append("PRIVMSG: error: no such user\n");
                }
            }
        }


    private:
        std::string                  _cmd;
        std::vector<std::string>     _param;
        User                        *_user;
        Tree                        *_tree;
        std::string                 _pass;

};
#endif

/*

1. check for \r\n
2. take evrything before and extract if, memmove, if empty return
3. split by ws
4. fill in the fields (cmd + args)
5. do reg status check at 3
6. if cmd doest exist error
7. switch commands
    1. Pass, if no arg error, if wrong arg error, if right arg reg status +1
    2. nick, if no arg error, if non compliant error, if already a nick or channel name, error, else 1. put im _nickname 2. map. modify nickname 3. regstat+1
    3. user, idem, idem, else 1. update fields 2. regstat+1
    4. privmsg, if no target error, if non existing target error, else send to user / channel
    5. join, check if channel exist, check if not ban,

    ...
    x. quit, send quit msg to all member channel, remove user from tree, return

*/
