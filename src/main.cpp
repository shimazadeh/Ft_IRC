#include "../inc/ft_irc.hpp"

// int	main(void)
// {
// 	//create a socket (IPv4, TCP)
// 	int	sockfd = socket(AF_INET, SOCK_STREAM, 0);
// 	if (sockfd == -1)
// 	{
// 		std::cout << "Failed to create socket, errno: " << errno << std::endl;
// 		exit(EXIT_FAILURE);
// 	}

// 	// Listen to port 9999 on any address
// 	sockaddr_in sockaddr;
// 	sockaddr.sin_family = AF_INET;
// 	sockaddr.sin_addr.s_addr = INADDR_ANY;
// 	sockaddr.sin_port = htons(9999); // htons is necessary to convert a number to
// 									// network byte order
// 	if (bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) {
// 		std::cout << "Failed to bind to port 9999. errno: " << errno << std::endl;
// 		exit(EXIT_FAILURE);
// 	}
// 	// Start listening. Hold at most 10 connections in the queue
// 	if (listen(sockfd, 10) < 0) {
// 		std::cout << "Failed to listen on socket. errno: " << errno << std::endl;
// 		exit(EXIT_FAILURE);
// 	}
// 	while(1)
// 	{
// 		// Grab a connection from the queue
// 		auto addrlen = sizeof(sockaddr);
// 		int connection = accept(sockfd, (struct sockaddr*)&sockaddr, (socklen_t*)&addrlen);
// 		if (connection < 0) {
// 			std::cout << "Failed to grab connection. errno: " << errno << std::endl;
// 			exit(EXIT_FAILURE);
// 		}
// 		std::cout << "LOL " << std::endl;

// 		// Read from the connection
// 		char buffer[100];
// 		auto bytesRead = read(connection, buffer, 100);
// 		while(bytesRead > 0)
// 		{
// 			std::cout << "The message from " << connection << ": " << buffer;
// 			bytesRead = read(connection, buffer, 100);
// 		}

// 		// Send a message to the connection
// 		std::string response = "Good talking to you\n";
// 		send(connection, response.c_str(), response.size(), 0);

// 		// Close the connections
// 		close(connection);
// 		usleep(1000);
// 	}
// 	close(sockfd);
// }
//https://www.tutorialspoint.com/unix_sockets/network_host_names.htm
//nc 127.0.0.1 port#

int	main(void)
{
	int    len, rc, on = 1;
	int    listen_sd = -1, new_sd = -1;
	int    end_server = FALSE, compress_array = FALSE;
	int    close_conn;
	char   buffer[80];
	struct sockaddr_in6   addr;
	int    timeout;
	struct pollfd fds[200];
	int    nfds = 1, current_size = 0, i, j;

	listen_sd = socket(AF_INET6, SOCK_STREAM, 0);
	if (listen_sd < 0)
	{
		perror("socket() failed");
		exit(-1);
	}

	rc = setsockopt(listen_sd, SOL_SOCKET,  SO_REUSEADDR, (char *)&on, sizeof(on));
	if (rc < 0)
	{
		perror("setsockopt() failed");
		close(listen_sd);
		exit(-1);
	}

	rc = ioctl(listen_sd, FIONBIO, (char *)&on);
	if (rc < 0)
	{
		perror("ioctl() failed");
		close(listen_sd);
		exit(-1);
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin6_family      = AF_INET6;
	memcpy(&addr.sin6_addr, &in6addr_any, sizeof(in6addr_any));
	addr.sin6_port        = htons(SERVER_PORT);
	rc = bind(listen_sd, (struct sockaddr *)&addr, sizeof(addr));
	if (rc < 0)
	{
		perror("bind() failed");
		close(listen_sd);
		exit(-1);
	}

	rc = listen(listen_sd, 32);
	if (rc < 0)
	{
		perror("listen() failed");
		close(listen_sd);
		exit(-1);
	}

 	memset(fds, 0 , sizeof(fds));

	fds[0].fd = listen_sd;
	fds[0].events = POLLIN;
	timeout = (3 * 60 * 1000);

	do
	{
		/* Call poll() and wait 3 minutes for it to complete.      */
		rc = poll(fds, nfds, timeout);

		/* Check to see if the poll call failed.                   */
		if (rc < 0)
		{
			perror("poll() failed");
			break;
		}

		/* Check to see if the 3 minute time out expired.          */
		if (rc == 0)
		{
			printf("poll() timed out. End program.\n");
			break;
		}

		current_size = nfds;
		for (i = 0; i < current_size; i++)
		{
			/*********************************************************/
			/* Loop through to find the descriptors that returned    */
			/* POLLIN and determine whether it's the listening       */
			/* or the active connection.                             */
			/*********************************************************/
			if(fds[i].revents == 0)
				continue;

			/*********************************************************/
			/* If revents is not POLLIN, it's an unexpected result,  */
			/* log and end the server.                               */
			/*********************************************************/
			if(fds[i].revents != POLLIN)
			{
				printf("  Error! revents = %d\n", fds[i].revents);
				end_server = TRUE;
				break;

			}
			if (fds[i].fd == listen_sd)
			{
				/*******************************************************/
				/* Listening descriptor is readable.                   */
				/*******************************************************/
				printf("  Listening socket is readable\n");

				/*******************************************************/
				/* Accept all incoming connections that are            */
				/* queued up on the listening socket before we         */
				/* loop back and call poll again.                      */
				/*******************************************************/
				do
				{
				/*****************************************************/
				/* Accept each incoming connection. If               */
				/* accept fails with EWOULDBLOCK, then we            */
				/* have accepted all of them. Any other              */
				/* failure on accept will cause us to end the        */
				/* server.                                           */
				/*****************************************************/
				new_sd = accept(listen_sd, NULL, NULL);
				if (new_sd < 0)
				{
					if (errno != EWOULDBLOCK)
					{
						perror("  accept() failed");
						end_server = TRUE;
					}
					break;
				}

				/*****************************************************/
				/* Add the new incoming connection to the            */
				/* pollfd structure                                  */
				/*****************************************************/
				printf("  New incoming connection - %d\n", new_sd);
				fds[nfds].fd = new_sd;
				fds[nfds].events = POLLIN;
				nfds++;

				/*****************************************************/
				/* Loop back up and accept another incoming          */
				/* connection                                        */
				/*****************************************************/
				} while (new_sd != -1);
			}

			/*********************************************************/
			/* This is not the listening socket, therefore an        */
			/* existing connection must be readable                  */
			/*********************************************************/

			else
			{
				printf("  Descriptor %d is readable\n", fds[i].fd);
				close_conn = FALSE;
				/*******************************************************/
				/* Receive all incoming data on this socket            */
				/* before we loop back and call poll again.            */
				/*******************************************************/

				do
				{
				/*****************************************************/
				/* Receive data on this connection until the         */
				/* recv fails with EWOULDBLOCK. If any other         */
				/* failure occurs, we will close the                 */
				/* connection.                                       */
				/*****************************************************/
				rc = recv(fds[i].fd, buffer, sizeof(buffer), 0);
				if (rc < 0)
				{
					if (errno != EWOULDBLOCK)
					{
					perror("  recv() failed");
					close_conn = TRUE;
					}
					break;
				}

				/*****************************************************/
				/* Check to see if the connection has been           */
				/* closed by the client                              */
				/*****************************************************/
				if (rc == 0)
				{
					printf("  Connection closed\n");
					close_conn = TRUE;
					break;
				}

				/*****************************************************/
				/* Data was received                                 */
				/*****************************************************/
				len = rc;
				printf("  %d bytes received\n", len);

				/*****************************************************/
				/* Echo the data back to the client                  */
				/*****************************************************/
				rc = send(fds[i].fd, buffer, len, 0);
				if (rc < 0)
				{
					perror("  send() failed");
					close_conn = TRUE;
					break;
				}

				} while(TRUE);

				/*******************************************************/
				/* If the close_conn flag was turned on, we need       */
				/* to clean up this active connection. This            */
				/* clean up process includes removing the              */
				/* descriptor.                                         */
				/*******************************************************/
				if (close_conn)
				{
					close(fds[i].fd);
					fds[i].fd = -1;
					compress_array = TRUE;
				}
			}  /* End of existing connection is readable             */
		} /* End of loop through pollable descriptors              */
		if (compress_array)
		{
			compress_array = FALSE;
			for (i = 0; i < nfds; i++)
			{
				if (fds[i].fd == -1)
				{
				for(j = i; j < nfds; j++)
				{
					fds[j].fd = fds[j+1].fd;
				}
				i--;
				nfds--;
				}
			}
		}

	} while (end_server == FALSE); /* End of serving running.    */

	/*************************************************************/
	/* Clean up all of the sockets that are open                 */
	/*************************************************************/
	for (i = 0; i < nfds; i++)
	{
		if(fds[i].fd >= 0)
			close(fds[i].fd);
	}
}
