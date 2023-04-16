# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: shabibol <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/07/02 17:41:06 by shabibol          #+#    #+#              #
#    Updated: 2022/07/02 17:41:08 by shabibol         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

TARGET   = Ft_IRC

CC       = g++

CFLAGS   = -Wall -Wextra -Werror -g3 -std=c++98 -std=c++11

LINKER   = g++

SRCDIR   = src/

OBJDIR   = obj/

SOURCES  := src/main.cpp

OBJECTS  := $(SOURCES:$(SRCDIR)%.cpp=$(OBJDIR)%.o)

INCLUDES := inc/ft_irc.hpp

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
