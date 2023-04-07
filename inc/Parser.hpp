#ifndef _PARSER_HPP
#define _PARSER_HPP

#include "ft_irc.hpp"
#include "User.hpp"

class Parser
{
    public:
        Parser(User &user, Tree &tree);
        ~Parser();

        void    check_for_cmd();
        void    tokenizer();
        void    execute();

        //in the commands you will mostly use find, erase, insert functions of map to execute
        void    join();
        void    quit();


    private:
        std::string     _cmd;
        std::string     _param[];
        User            &_user;
        Tree            &_tree;




};
#endif
