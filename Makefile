# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: yanaranj <yanaranj@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/11/13 16:35:30 by yanaranj          #+#    #+#              #
#    Updated: 2025/11/24 20:00:49 by yanaranj         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NC = \033[0m
RED = \e[1;91m
GREEN = \e[1;92m
YELLOW = \e[1;93m
BLUE = \e[1;94m
TURQUOISE = \e[1;38;5;80m
FUCSIA = \e[1;38;5;201m
PURPLE = \e[1;38;5;135m
ORANGE = \e[1;38;2;255;128;0m

NAME = ircserv

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98
RM = rm -f


INCDIR = ./includes/
SRCSDIR = ./srcs/
OBJSDIR = ./objs/

INC = -I $(INCDIR)Server.hpp \
	  -I $(INCDIR)Exceptions.hpp \

SRCS = $(SRCSDIR)Server.cpp
OBJS = $(SRCS:$(SRCSDIR)%.cpp=$(OBJSDIR)%.o)


all: $(NAME)
	@mkdir -p $(OBJSDIR)

$(NAME): $(OBJS)
	@echo "$(GREEN)[✔] $(NAME) compiled successfully!$(NC)"
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)


clean:
	@$(RM) $(OBJS)
	@echo "$(RED)objects had been removed"

fclean: clean
	@$(RM) $(NAME)
	@echo "$(RED)Bye!🫰"

re: fclean all

.PHONY: all clean fclean re