# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jeannecolmou <jeannecolmou@student.42.f    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/07/02 17:41:06 by shabibol          #+#    #+#              #
#    Updated: 2023/04/19 12:01:38 by jeannecolmo      ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

TARGET   = ircserv

CC       = g++

CFLAGS   = -Wall -Wextra -Werror -g3 -std=c++98

LINKER   = g++

SRCDIR   = src/

OBJDIR   = obj/

SOURCES  := src/Channel.cpp  src/main.cpp  src/Parser.cpp  src/Server.cpp  src/Tree.cpp  src/User.cpp

OBJECTS  := $(SOURCES:$(SRCDIR)%.cpp=$(OBJDIR)%.o)

INCLUDES := inc/Channel.hpp  inc/ft_irc.hpp  inc/Parser.hpp  inc/Server.hpp  inc/Tree.hpp  inc/User.hpp

rm       = rm -f

$(TARGET): $(OBJECTS)
	$(LINKER) $(OBJECTS) -o $@

$(OBJECTS): $(OBJDIR)%.o : $(SRCDIR)%.cpp $(INCLUDES)
	$(CC) $(CFLAGS) -c $< -o $@

all: $(TARGET)

clean:
	$(rm) $(OBJECTS)

fclean: clean
	$(rm) $(TARGET)

re: fclean all

.PHONY: all clean fclean re
