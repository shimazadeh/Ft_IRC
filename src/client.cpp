#include "../inc/ft_irc.hpp"


int main(void)
{
	int	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		std::cout << "Failed to create socket, errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}
	// connect
	return (0);
}
