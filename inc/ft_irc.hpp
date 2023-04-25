#ifndef _FT_IRC_HPP
#define _FT_IRC_HPP

#include <sys/socket.h> 
#include <netinet/in.h> 
#include <iostream> 
#include <unistd.h> 
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <cstring>
#include <stdexcept>

#include <map>
#include <vector>
#include <utility>

#define TIMEOUT 10
#define BUFFER_SIZE 100
#define SERVER_NAME ircserv
#endif
