#include "../inc/ft_irc.hpp"
#include "../inc/Server.hpp"

bool check_arg(int ac, const char **av)
{
	size_t i = 0;
	if (ac != 3)
		return (false);
	while (i < strlen(av[1]))
	{
		if (!isdigit(av[1][i]))
			return (false);
		++i;
	}
	if (i > 5 || (i == 5 && strcmp(av[1], "65535") > 0))
		return (false);
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

//https://www.tutorialspoint.com/unix_sockets/network_host_names.htm
//nc 127.0.0.1 port#
