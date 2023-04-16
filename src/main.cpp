#include "../inc/ft_irc.hpp"
#include "../inc/Server.hpp"

int	main(int ac, char **av)
{
	if (!check_arg(ac, av))
		return 1;

	Server	irc_serv(atoi(av[1]), av[2]);

	irc_serv.set_up();
	irc_serv.loop();
	irc_serv.serv_exit();

	return 0;
}

//https://www.tutorialspoint.com/unix_sockets/network_host_names.htm
//nc 127.0.0.1 port#
