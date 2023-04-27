#include "../inc/ft_irc.hpp"
#include "../inc/Server.hpp"

#define PORT_LIMIT "65535"

bool check_arg(int ac, const char **av)
{
	size_t i = 0;
	if (ac != 3)
	{
		std::cout << "Error: Wrong number of arguments, expected 2, given " << ac - 1 << std::endl;
		return (false);
	}
	while (i < strlen(av[1]))
	{
		if (!isdigit(av[1][i]))
		{
			std::cout << "Error: Port argument must contain only digits" << std::endl;
			return (false);
		}
		++i;
	}
	if (i > 5 || (i == 5 && strcmp(av[1], PORT_LIMIT) > 0))
	{
		std::cout << "Error: Port argument must be in the range 0-" << PORT_LIMIT << std::endl;
		return (false);
	}
	return (true);
}

int	main(int ac, const char **av)
{
	if (!check_arg(ac, av))
		return 1;

	Server	irc_serv(av[2], atoi(av[1]));

	irc_serv.set_up();
	irc_serv.loop();
	irc_serv.serv_exit(0);

	return 0;
}
/*
PASS <ARG>

NICK <ARG>

USER <username> 0 * <realname>

PING <ARG>

OPER <name> <pass>

QUIT [reason]

JOIN <channel>{,<channel>} [<key>{,<key>}]

PART <channel>,<channnel> [reason]

TOPIC <channel> [topic]

NAMES <channel>,<channel>

KICK <channel> <user>,<user> [comment]

PRIVMSG <target>,<target> <text to be sent>

NOTICE <target>,<target> <text to be sent>

KILL <nickname> <comment> //fixed: connect with nc try to kill a non-existing user ->seg fault
*/
