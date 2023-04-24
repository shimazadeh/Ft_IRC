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
and fixed their formats, the program is way cleaner now. I fixed the parsing
when we use nc with irssi client.

problems Im not sure of:

	1.Kick works and it doesnt allow the user to send msg to the channel. the only issue is that the channel window
	is still open but if you try to join the same channel the client doesnt even send the command to our server.
	my theory is that the user needs to get invited if they get kicked out thats why irssi ignores the JOIN
	command after kick. Kick requires you to be an IRC operator. we can use this justification but idk if the channel
	window not closing is a problem(?)

	2.KILL works but if the user is inside the channel the msg is not sent in
	the channel but in the main window it says connection lost. not sure if its a problem(?)

	5. TOPIC doesnt work, I could not figure this one out

new stuff I found and fixed:
	1. there was a bug that if you join multiple people in the same channel,
	the last person who joined will recieved the correct list of names
	this was because of the format of JOIN msg. its fixed now.

	2. NAMES: works now , with irssi will output the result inside the channel window,
	also works with nc.

	3. List works only outside the channel window which is normal,
	I wrote an itoa function to use cuz I couldnt figure out the
	itoa library problem.

	4.I tested things running two irssi client and one nc client, if you have any ideas for the
	above problems maybe you can fix them? also can you run through the correction sheet one
	more time and find bugs and tell me what are the problems.


as of now we have 5 channel operations that are working: JOIN, PART, NAMES, LIST, KICK(IS NOT PERFECT)
what I ask you for tmw is to go through the IRC correction sheet and test the shit out of this.
if you come early and test maybe we can push between 2-5 and be done. if you cannot come early just test
between 2-5 and we fix and push on Wednesday. I just want to make sure it passes the correction because
if we fail I have to wait another 3 days to repush and Im on a time crunch.
*/
