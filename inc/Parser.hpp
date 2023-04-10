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

        void    check_for_cmd();//go to the user object look for rbuff , check if there is valid command
        void    tokenizer();//parse it and fill in the _cmd and _param
        void    execute()//will check what cmd it is and it will call the cmd
        {
            if (_cmd.compare("JOIN"))
                join();
            else if //.....
        }

        //in the commands you will mostly use find, erase, insert functions of map to execute
        void    join();//go the tree and modify the channel tree
        void    quit();
        // ...


    private:
        std::string     _cmd;
        std::string     _param[];
        User            *_user;
        Tree            *_tree;




};
#endif
