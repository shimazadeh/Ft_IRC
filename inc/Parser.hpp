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
        Parser(Tree& tree, std::string password);
        ~Parser();

        void                        change_user(User    *username);
        User                        *get_user();
        int                         check_for_cmd(int c, std::vector<bool>& closecon, std::vector<struct pollfd>& _fds);
        void                        fill_in_params(std::string  buf);
        std::vector<std::string>    custom_split(std::string buf);

        void    execute(int c, std::vector<bool>& closecon, std::vector<struct pollfd>& _fds);
        void    pass();
        void    cap();
        void    nick();
        void    user();
        void    ping();
        void    oper();
        bool    quit();
        void    part();
        void    topic();
        void    who();
        void    names();
        void    kick();
        void    join();
        void    privmsg();
        void    notice();
        void    mode();
        void    kill(std::vector<bool>& closecon, std::vector<struct pollfd>& _fds);

    private:
        std::string                  _cmd;
        std::vector<std::string>     _param;
        User                        *_user;
        Tree                        *_tree;
        std::string                 _pass;

};
char	*ft_itoa(int nb);

#endif

/*

1. check for \n
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
