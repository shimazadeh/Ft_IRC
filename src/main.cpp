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

//https://www.tutorialspoint.com/unix_sockets/network_host_names.htm
//nc 127.0.0.1 port#

/*DEBUGGING

I kinda understood format of the messages so I went through all the errors
and fixed their formats, I fixed the parsing when we use nc with irssi
client. below are the problems that I still have:

1.Kick works and it doesnt allow the user to send msg to the channel but the channel window is open
and if you try to join the same channel the client doesnt even send the command to our server ??? my theory is
that the user needs to get invited if they get kicked out thats why irssi ignores the JOIN command after kick

2.KILL works but if the user is inside the channel the msg is not sent in
the channel but in the main window it says connection lost

3. NAMES: will only send the name command when you are in the channel
window, it never sends the "/NAME " command to our server. but there is a bug
that if you join two people in the same channel, the second person who joined
will recieved the correct list of names but the first person only gets
the old list of names

4. NAMES: if we test this with nc it works perfectly fine we could somehow
justify this because idk why irssi doesnt send the command to us and it execute
it by itself

5. TOPIC doesnt work, I could not figure this one out

6. List works only outside the channel window which is normal,
I wrote an itoa function to use cuz I couldnt figure out the
itoa library problem.

I tested things running two irssi client and one nc client, if you have any ideas for the
above problems maybe you can fix them? also can you run through the correction sheet one
more time and find bugs and tell me what are the problems.
*/
