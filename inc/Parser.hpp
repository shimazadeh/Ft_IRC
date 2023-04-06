#ifndef _PARSER_HPP
#define _PARSER_HPP

#include "ft_irc.hpp"

class Parser
{
    public:
        Parser();
        ~Parser();

        void    check_for_cmd();
        void    tokenizer();
        void    execute();



    private:
        std::string     _cmd;
        std::string     _param[];





};
#endif
