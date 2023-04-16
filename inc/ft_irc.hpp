#ifndef _FT_IRC_HPP
#define _FT_IRC_HPP

#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <iostream> // For cout
#include <unistd.h> // For read
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <cstring>
#include <stdexcept>

#include <map>
#include <vector>
#include <unordered_map>
#include <utility>


// #include "User.hpp"
// #include "Parser.hpp"
// #include "Tree.hpp"
// #include "Server.hpp"
// #include "Channel.hpp"


#define TIMEOUT 3
#define BUFFER_SIZE 1024
#endif
